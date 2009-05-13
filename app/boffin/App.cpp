/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include <boost/bind.hpp>
#include <QFileDialog>
#include <QMenu>
#include <QComboBox>
#include <QLabel>
#include <QShortcut>
#include <QVBoxLayout>
#include <phonon/audiooutput.h>
#include <phonon/backendcapabilities.h>
#include <lastfm/NetworkAccessManager>
#include "lib/unicorn/QMessageBoxBuilder.h"
#include "App.h"
#include "MainWindow.h"
#include "MediaPipeline.h"
#include "PickDirsDialog.h"
#include "ScanProgressWidget.h"
#include "ScrobSocket.h"
#include "TrackSource.h"
#include "PlaydarStatus.h"
#include "BoffinRqlRequest.h"
#include "PlaydarTagCloudModel.h"


#define OUTPUT_DEVICE_KEY "OutputDevice"


App::App( int& argc, char** argv )
   : unicorn::Application( argc, argv )
   , m_mainwindow( 0 )
   , m_tagcloud( 0 )
   , m_scrobsocket( 0 )
   , m_pipe( 0 )
   , m_audioOutput( 0 )
   , m_playing( false )
   , m_api( "http://localhost:8888", "fd91e3fb-311d-4903-91d7-87af53281d3f" )
{
    m_wam = new lastfm::NetworkAccessManager( this );
    m_playdarStatus = new PlaydarStatus(m_wam, m_api);
}


App::~App()
{
    cleanup();
    if (m_audioOutput) QSettings().setValue( OUTPUT_DEVICE_KEY, m_audioOutput->outputDevice().name() );
    delete m_pipe;
}


void
App::cleanup()
{
}


void
App::init( MainWindow* window ) throw( int /*exitcode*/ )
{
    m_mainwindow = window;
    
    window->ui.play->setEnabled( false );
    window->ui.pause->setEnabled( false );
    window->ui.skip->setEnabled( false );
    window->ui.wordle->setEnabled( false );

////// radio
    QString const name = QSettings().value( OUTPUT_DEVICE_KEY ).toString();
    m_audioOutput = new Phonon::AudioOutput( Phonon::MusicCategory, this );

    QActionGroup* actiongroup = new QActionGroup( window->ui.outputdevice );

    foreach (Phonon::AudioOutputDevice d, Phonon::BackendCapabilities::availableAudioOutputDevices()) 
    {
        QAction* a = window->ui.outputdevice->addAction( d.name() );
        a->setCheckable( true );
        if (name == d.name())
            m_audioOutput->setOutputDevice( d );
        if (m_audioOutput->outputDevice().name() == d.name())
            a->setChecked( true );
            
        actiongroup->addAction( a );
    }

	m_audioOutput->setVolume( 1.0 /* Settings().volume() */ );

    connect( actiongroup, SIGNAL(triggered( QAction* )), SLOT(onOutputDeviceActionTriggered( QAction* )) );
    
	m_pipe = new MediaPipeline( m_audioOutput, this );
    connect( m_pipe, SIGNAL(preparing()), SLOT(onPreparing()) );
    connect( m_pipe, SIGNAL(started( Track )), SLOT(onStarted( Track )) );
    connect( m_pipe, SIGNAL(paused()), SLOT(onPaused()) );
    connect( m_pipe, SIGNAL(resumed()), SLOT(onResumed()) );
    connect( m_pipe, SIGNAL(stopped()), SLOT(onStopped()) );
    connect( m_pipe, SIGNAL(error( QString )), SLOT(onPlaybackError( QString )) );

    m_scrobsocket = new ScrobSocket( this );
    connect( m_pipe, SIGNAL(started( Track )), m_scrobsocket, SLOT(start( Track )) );
    connect( m_pipe, SIGNAL(paused()), m_scrobsocket, SLOT(pause()) );
    connect( m_pipe, SIGNAL(resumed()), m_scrobsocket, SLOT(resume()) );
    connect( m_pipe, SIGNAL(stopped()), m_scrobsocket, SLOT(stop()) );

/// parts of the scanning stuff
    //m_trackTagUpdater = TrackTagUpdater::create(
    //        "http://musiclookup.last.fm/trackresolve",
    //        100,        // number of days track tags are good 
    //        5);         // 5 minute delay between web requests

/// connect
    connect( window->ui.play, SIGNAL(triggered()), SLOT(play()) );
    connect( window->ui.pause, SIGNAL(toggled( bool )), m_pipe, SLOT(setPaused( bool )) );
    connect( window->ui.skip, SIGNAL(triggered()), m_pipe, SLOT(skip()) );
    connect( window->ui.rescan, SIGNAL(triggered()), SLOT(startAgain()) );
    connect( window->ui.xspf, SIGNAL(triggered()), SLOT(xspf()) );
    connect( m_mainwindow->ui.wordle, SIGNAL( triggered()), SLOT( onWordle()));

    QShortcut* cut = new QShortcut( Qt::Key_Space, window );
    connect( cut, SIGNAL(activated()), SLOT(playPause()) );
    cut->setContext( Qt::ApplicationShortcut );

    onScanningFinished();

/// go!
    //if (!scan( false ))
    //    throw 1; //abort app
}


void
App::onOutputDeviceActionTriggered( QAction* a )
{
    //FIXME for some reason setOutputDevice just returns false! :(
    
    QString const name = a->text();
    
    foreach (Phonon::AudioOutputDevice d, Phonon::BackendCapabilities::availableAudioOutputDevices())
        if (d.name() == name) {
            qDebug() << m_audioOutput->setOutputDevice( d );
            qDebug() << m_audioOutput->outputDevice().name();
            return;
        }
}

#include "TagCloudWidget.h"
#include "TagCloudView.h"
#include "TagDelegate.h"
#include "PlaydarTagCloudModel.h"
#include "PlaydarStatRequest.h"
#include "PlaydarStatus.h"


void
App::onScanningFinished()
{    
    disconnect( sender(), 0, this, 0 ); //only once pls
    
    m_mainwindow->ui.play->setEnabled( true );
    m_mainwindow->ui.pause->setEnabled( false );
    m_mainwindow->ui.skip->setEnabled( false );
    m_mainwindow->ui.rescan->setEnabled( true );    
    m_mainwindow->ui.wordle->setEnabled( true );
    m_mainwindow->ui.playdarHosts->setModel( m_playdarStatus->hostsModel() );

    connect(m_playdarStatus, SIGNAL(changed(QString)), m_mainwindow->ui.playdarStatus, SLOT(setText(QString)));
    connect(m_playdarStatus, SIGNAL(connected()), SLOT(onPlaydarConnected()));
    m_playdarStatus->start();
}

void
App::onPlaydarConnected()
{
    m_tagcloud = new TagCloudWidget( boost::bind(&App::createTagCloudModel, this), "dougma" );
//    m_tagcloud->setFrameStyle( QFrame::NoFrame );
    m_mainwindow->setCentralWidget( m_tagcloud );
}


PlaydarTagCloudModel* 
App::createTagCloudModel()
{
    return new PlaydarTagCloudModel(m_api, m_wam);
}

void
App::play()
{
    QString s = m_tagcloud->rql();
    int ii = 0;
}


void
App::onReadyToPlay()
{
    m_pipe->play( (TrackSource*) sender() );
}


void
App::playPause()
{
    if (m_playing)
        m_mainwindow->ui.pause->toggle();
    else
        play();
}


void
App::xspf()
{
    QString path = QFileDialog::getOpenFileName( m_mainwindow, "Open XSPF File", "*.xspf" );
    if (path.size())
    {
        m_mainwindow->QMainWindow::setWindowTitle( "Resolving XSPF..." );
 //       m_pipe->playXspf( path );
    }
}


void
App::onPreparing() //MediaPipeline is preparing to play a new station
{
    m_mainwindow->QMainWindow::setWindowTitle( "Boffing up..." );
        
    QAction* a = m_mainwindow->ui.play;
    a->setIcon( QPixmap(":/stop.png") );
    disconnect( a, SIGNAL(triggered()), this, 0 );
    connect( a, SIGNAL(triggered()), m_pipe, SLOT(stop()) );
}


void
App::onStarted( const Track& t )
{
    m_playing = true; // because phonon is shit and we can't rely on its state
    
    m_mainwindow->setWindowTitle( t );
    m_mainwindow->ui.play->blockSignals( true );
    m_mainwindow->ui.play->setChecked( true );
    m_mainwindow->ui.play->blockSignals( false );
    m_mainwindow->ui.pause->blockSignals( true );
    m_mainwindow->ui.pause->setChecked( false );
    m_mainwindow->ui.pause->setEnabled( true );
    m_mainwindow->ui.pause->blockSignals( false );
    
    m_mainwindow->ui.skip->setEnabled( true );
}


void
App::onPaused()
{
    m_mainwindow->ui.pause->blockSignals( true );
    m_mainwindow->ui.pause->setChecked( true );
    m_mainwindow->ui.pause->blockSignals( false );
}


void
App::onResumed()
{
    m_mainwindow->ui.pause->blockSignals( true );
    m_mainwindow->ui.pause->setChecked( false );
    m_mainwindow->ui.pause->blockSignals( false );
}


void
App::onStopped()
{
    m_mainwindow->setWindowTitle( Track() );
    m_mainwindow->ui.play->blockSignals( true );
    m_mainwindow->ui.play->setChecked( false );
    m_mainwindow->ui.play->blockSignals( false );
    m_mainwindow->ui.pause->blockSignals( true );
    m_mainwindow->ui.pause->setChecked( false );
    m_mainwindow->ui.pause->setEnabled( false );
    m_mainwindow->ui.pause->blockSignals( false );
    
    m_mainwindow->ui.skip->setEnabled( false );

    QAction* a = m_mainwindow->ui.play;
    a->setIcon( QPixmap(":/play.png") );
    disconnect( a, SIGNAL(triggered()), m_pipe, 0 );
    connect( a, SIGNAL(triggered()), SLOT(play()) );
    
    m_playing = false;
}


void
App::onPlaybackError( const QString& msg )
{
    //TODO: need to make this more like client 2's subtle yellow box.
    QMessageBoxBuilder( m_mainwindow )
            .setTitle( "Playback Error" )
            .setText( msg )
            .exec();
}


#include "WordleDialog.h"
void
App::onWordle()
{
    static OneDialogPointer<WordleDialog> d;
    if(!d) {
        d = new WordleDialog( m_mainwindow );
        QString output;
        //TagCloudModel model( this, 0 );
        //for(int i = 0; i < model.rowCount(); ++i) {
        //    QModelIndex index = model.index( i, 0 );
        //    QString weight = index.data( TagCloudModel::WeightRole ).toString();
        //    QString tag = index.data().toString().trimmed().simplified().replace( ' ', '~' );
        //    output += tag + ':' + weight + '\n';
        //}
        d->setText( output );
    }
    d.show();
}

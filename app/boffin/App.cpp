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
 
#include "App.h"
#include "MainWindow.h"
#include "MediaPipeline.h"
#include "PickDirsDialog.h"
#include "ScanProgressWidget.h"
#include "ScrobSocket.h"
#include "app/clientplugins/localresolver/LocalContentScannerThread.h"
#include "app/clientplugins/localresolver/LocalContentScanner.h"
#include "app/clientplugins/localresolver/LocalContentConfigurator.h"
#include "app/clientplugins/localresolver/TrackTagUpdater.h"
#include "app/clientplugins/localresolver/QueryError.h"
#include "lib/unicorn/QMessageBoxBuilder.h"
#include <QFileDialog>
#include <QMenu>
#include <QVBoxLayout>
#include <phonon/audiooutput.h>
#include <phonon/backendcapabilities.h>

#define OUTPUT_DEVICE_KEY "OutputDevice"


App::App( int& argc, char** argv )
   : unicorn::Application( argc, argv )
   , m_contentScannerThread( 0 )
   , m_contentScanner( 0 )
   , m_trackTagUpdater( 0 )
   , m_mainwindow( 0 )
   , m_cloud( 0 )
   , m_scrobsocket( 0 )
   , m_pipe( 0 )
   , m_audioOutput( 0 )
{}


App::~App()
{
    cleanup();
    if (m_audioOutput) QSettings().setValue( OUTPUT_DEVICE_KEY, m_audioOutput->outputDevice().name() );
    delete m_pipe;
}


void
App::cleanup()
{
    if (m_contentScanner) m_contentScanner->stop();
    if (m_contentScannerThread) m_contentScannerThread->wait();
    delete m_contentScanner;
    delete m_contentScannerThread;
}


void
App::init( MainWindow* window ) throw( int /*exitcode*/ )
{
    m_mainwindow = window;
    
    window->ui.play->setEnabled( false );
    window->ui.pause->setEnabled( false );
    window->ui.skip->setEnabled( false );
        
////// radio
    QString const name = QSettings().value( OUTPUT_DEVICE_KEY ).toString();
    m_audioOutput = new Phonon::AudioOutput( Phonon::MusicCategory, this );
	m_audioOutput->setVolume( 1.0 /* Settings().volume() */ );

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
    m_trackTagUpdater = TrackTagUpdater::create(
            "http://musiclookup.last.fm/trackresolve",
            100,        // number of days track tags are good 
            5);         // 5 minute delay between web requests

/// connect
    connect( window->ui.play, SIGNAL(triggered()), SLOT(play()) );
    connect( window->ui.pause, SIGNAL(toggled( bool )), m_pipe, SLOT(setPaused( bool )) );
    connect( window->ui.skip, SIGNAL(triggered()), m_pipe, SLOT(skip()) );
    connect( window->ui.rescan, SIGNAL(triggered()), SLOT(startAgain()) );
    connect( window->ui.xspf, SIGNAL(triggered()), SLOT(xspf()) );

/// go!
    if (!scan( false ))
        throw 1; //abort app
}


void
App::startAgain()
{
    scan( true );
}


bool
App::scan( bool delete_all_files_first )
{
/// content scanner
    try
    {
        LocalContentConfigurator cfg;

        if ( delete_all_files_first ) 
        {
            cfg.deleteAllFiles();
        }

        QStringList const dirs = cfg.getScanDirs();
        if (dirs.isEmpty() || cfg.getFileCount() == 0)
        {
            PickDirsDialog picker( m_mainwindow );
            picker.setDirs( dirs );
            if (picker.exec() == QDialog::Rejected)
                return false;        // abort the whole app
            cfg.changeScanDirs( picker.dirs() );
            cfg.updateVolumeAvailability();
        }
    } 
    catch (QueryError e)
    {
        // the db is probably an old version
        qCritical() << "Database problem: " + e.text();
        
        QMessageBoxBuilder( m_mainwindow )
                .setTitle( "Warning" )
                .setText( "Boffin suffered a database problem, consult the log for the gory details" )
                .exec();
        return false; // not much we can do.
    }

    cleanup();

    m_contentScanner = new LocalContentScanner;
    connect(m_contentScanner, SIGNAL(trackScanned(Track)), m_trackTagUpdater, SLOT(needsUpdate()));
    m_contentScannerThread = new LocalContentScannerThread(m_contentScanner);

////// scanning widget
    ScanProgressWidget* progress = new ScanProgressWidget;
    m_mainwindow->setCentralWidget( progress );
    connect( m_contentScanner, SIGNAL(trackScanned( Track )), progress, SLOT(onNewTrack( Track )) );
    connect( m_contentScanner, SIGNAL(dirScanStart( QString )), progress, SLOT(onNewDirectory( QString )) );
    connect( m_contentScanner, SIGNAL(finished()), progress, SLOT(onFinished()) );
    //queue so the progress widget can update its status label
    connect( m_contentScanner, SIGNAL(finished()), SLOT(onScanningFinished()), Qt::QueuedConnection );
    
    m_mainwindow->ui.rescan->setEnabled( false );

    m_contentScannerThread->start();
    
    return true;
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


#include "TagCloudView.h"
#include "TagDelegate.h"
#include "TagCloudModel.h"
void
App::onScanningFinished()
{    
    QTime time;
    time.start();
    
    qDebug() << "BEGIN tag cloud generation";
    
    disconnect( sender(), 0, this, 0 ); //only once pls
    
    m_cloud = new TagCloudView;
    m_cloud->setModel( new TagCloudModel );
    m_cloud->setItemDelegate( new TagDelegate );
    m_mainwindow->setCentralWidget( m_cloud );
    
    m_cloud->setFrameStyle( QFrame::NoFrame );

    qDebug() << "END:" << time.elapsed() << "ms";
    
    m_mainwindow->ui.play->setEnabled( true );
    m_mainwindow->ui.pause->setEnabled( false );
    m_mainwindow->ui.skip->setEnabled( false );
    m_mainwindow->ui.rescan->setEnabled( true );    
}


void
App::play()
{
    if (m_cloud) { 
        if( m_cloud->currentTags().isEmpty())
        {
            QMessageBoxBuilder( m_mainwindow ).setTitle( tr("No Tags Selected") )
                                             .setText( tr("Select at least one tag from the cloud below to start playing music." ))
                                             .sheet()
                                             .exec();
            return;
        }
        m_pipe->playTags( m_cloud->currentTags() );
        m_mainwindow->QMainWindow::setWindowTitle( "Boffing up..." );
    }
}


void
App::xspf()
{
    QString path = QFileDialog::getOpenFileName( m_mainwindow, "Open XSPF File", "*.xspf" );
    if (path.size())
    {
        m_mainwindow->QMainWindow::setWindowTitle( "Resolving XSPF..." );
        m_pipe->playXspf( path );
    }
}


void
App::onPreparing() //MediaPipeline is preparing to play a new station
{
    m_cloud->viewport()->setEnabled( false ); //prevent interaction until stop pushed
        
    QAction* a = m_mainwindow->ui.play;
    a->setIcon( QPixmap(":/stop.png") );
    disconnect( a, SIGNAL(triggered()), this, 0 );
    connect( a, SIGNAL(triggered()), m_pipe, SLOT(stop()) );
}


void
App::onStarted( const Track& t )
{
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
    m_cloud->viewport()->setEnabled( true );
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

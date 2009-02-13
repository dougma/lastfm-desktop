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
#include "PickDirsDialog.h"
#include "ScanProgressWidget.h"
#include "app/clientplugins/localresolver/LocalContentScannerThread.h"
#include "app/clientplugins/localresolver/LocalContentScanner.h"
#include "app/clientplugins/localresolver/LocalContentConfig.h"
#include "app/clientplugins/localresolver/LocalRqlPlugin.h"
#include "app/clientplugins/localresolver/TrackResolver.h"
#include "app/clientplugins/localresolver/TrackTagUpdater.h"
#include "app/clientplugins/localresolver/QueryError.h"
#include "app/client/Resolver.h"
#include "app/client/XspfResolvingTrackSource.h"
#include "app/client/LocalRql.h"
#include "app/client/LocalRadioTrackSource.h"
#include "lib/lastfm/radio/Radio.h"
#include "lib/unicorn/QMessageBoxBuilder.h"
#include <QMenu>
#include <QVBoxLayout>
#include <phonon/audiooutput.h>
#include <phonon/backendcapabilities.h>

#define OUTPUT_DEVICE_KEY "OutputDevice"


App::App( int& argc, char** argv )
   : Unicorn::Application( argc, argv )
   , m_contentScannerThread( 0 )
   , m_contentScanner( 0 )
   , m_trackTagUpdater( 0 )
   , m_localRql( 0 )
   , m_trackResolver( 0 )
   , m_radio( 0 )
   , m_resolver( 0 )
   , m_mainwindow( 0 )
   , m_cloud( 0 )
{}


App::~App()
{
    if (m_contentScanner) m_contentScanner->stop();
    if (m_contentScannerThread) m_contentScannerThread->wait();
    delete m_contentScanner;
    delete m_contentScannerThread;    
    
    if (m_radio) QSettings().setValue( OUTPUT_DEVICE_KEY, m_radio->audioOutput()->outputDevice().name() );
}


void
App::init( MainWindow* window ) throw( int /*exitcode*/ )
{
    m_mainwindow = window;
    
    window->ui.play->setEnabled( false );
    window->ui.pause->setEnabled( false );
    window->ui.skip->setEnabled( false );
    
    connect( window->ui.play, SIGNAL(toggled( bool )), SLOT(onPlayActionToggled( bool )) );
    
////// radio
    QString const name = QSettings().value( OUTPUT_DEVICE_KEY ).toString();
    Phonon::AudioOutput* audioOutput = new Phonon::AudioOutput( Phonon::MusicCategory, this );
	audioOutput->setVolume( 1.0 /* Settings().volume() */ );

    QActionGroup* actiongroup = new QActionGroup( window->ui.outputdevice );

    foreach (Phonon::AudioOutputDevice d, Phonon::BackendCapabilities::availableAudioOutputDevices()) 
    {
        QAction* a = window->ui.outputdevice->addAction( d.name() );
        a->setCheckable( true );
        if (name == d.name())
            audioOutput->setOutputDevice( d );
        if (audioOutput->outputDevice().name() == d.name())
            a->setChecked( true );
            
        actiongroup->addAction( a );
    }

    connect( actiongroup, SIGNAL(triggered( QAction* )), SLOT(onOutputDeviceActionTriggered( QAction* )) );
    
	m_radio = new Radio( audioOutput );
	
	
    connect( m_radio, SIGNAL(trackSpooled( lastfm::Track )), SLOT(onTrackSpooled( lastfm::Track )) );

/// content scanner
    try
    {
        LocalContentConfig cfg;
        QStringList const dirs = cfg.getScanDirs();
        
        if (dirs.isEmpty()) 
        {
            PickDirsDialog picker( window );
            picker.setDirs( cfg.getScanDirs() );
            if (picker.exec() == QDialog::Rejected)
                throw 1;
            cfg.setScanDirs( picker.getDirs() );
            cfg.updateVolumeAvailability();
        }
    } 
    catch (QueryError e)
    {
        //TODO this is unacceptable error handling
        // just let the rest of the app continue regardless? FIXME soon!
    }

    m_contentScanner = new LocalContentScanner;
    m_trackTagUpdater = TrackTagUpdater::create(
            "http://musiclookup.last.fm/trackresolve",
            100,        // number of days track tags are good 
            5);         // 5 minute delay between web requests
    connect(m_contentScanner, SIGNAL(trackScanned(Track, int, int)), m_trackTagUpdater, SLOT(needsUpdate()));

    m_contentScannerThread = new LocalContentScannerThread(m_contentScanner);
    m_contentScannerThread->start();

/// local rql
    m_localRqlPlugin = new LocalRqlPlugin;
    m_localRqlPlugin->init();
    m_localRql = new LocalRql( QList<ILocalRqlPlugin*>() << m_localRqlPlugin );

/// content resolver
    m_trackResolver = new TrackResolver;
    m_resolver = new Resolver( QList<ITrackResolverPlugin*>() << m_trackResolver );

////// scanning widget
    ScanProgressWidget* progress = new ScanProgressWidget;
    window->setCentralWidget( progress );
    connect( m_contentScanner, SIGNAL(trackScanned(Track, int, int)), progress, SLOT(onNewTrack( Track, int, int )) );
    connect( m_contentScanner, SIGNAL(finished()), SLOT(onScanningFinished()), Qt::QueuedConnection );
}


void
App::openXspf( QString filename )
{
    XspfResolvingTrackSource* src = new XspfResolvingTrackSource( m_resolver, filename );
    m_radio->play( RadioStation( "XSPF" ), src );
    src->start();
}


void
App::onOutputDeviceActionTriggered( QAction* a )
{
    //FIXME for some reason setOutputDevice just returns false! :(
    
    QString const name = a->text();
    
    foreach (Phonon::AudioOutputDevice d, Phonon::BackendCapabilities::availableAudioOutputDevices())
        if (d.name() == name) {
            qDebug() << m_radio->audioOutput()->setOutputDevice( d );
            qDebug() << m_radio->audioOutput()->outputDevice().name();
            return;
        }
}


#include "TagCloudView.h"
#include "TagDelegate.h"
#include "TagCloudModel.h"
void
App::onScanningFinished()
{
    static_cast<ScanProgressWidget*>(m_mainwindow->centralWidget())->onFinished();
    
    QTime time;
    time.start();
    
    qDebug() << "Hi!";
    
    disconnect( sender(), 0, this, 0 ); //only once pls
    
    m_cloud = new TagCloudView;
    m_cloud->setModel( new TagCloudModel );
    m_cloud->setItemDelegate( new TagDelegate );
    m_mainwindow->setCentralWidget( m_cloud );
    
    m_cloud->setFrameStyle( QFrame::NoFrame );

    qDebug() << "Bye!" << time.elapsed() << "ms";
    
    m_mainwindow->ui.play->setEnabled( true );
    m_mainwindow->ui.pause->setEnabled( true );
    m_mainwindow->ui.skip->setEnabled( true );
}


void
App::play()
{
    qDebug() << "HI";
    if (m_cloud) play( QStringList() << m_cloud->currentTag() );
}


void
App::play( QStringList tags )
{
    for (int i = 0; i < tags.count(); ++i)
        tags[i] = "tag:\"" + tags[i] + '"';
    QString const rql = tags.join( " or " );
    LocalRqlResult* result = m_localRql->startParse( rql );
    
    if (!result)
    {
        MessageBoxBuilder( m_mainwindow )
                .setTitle( tr("Local Radio Error") )
                .setText( tr("Could not load plugin") )
                .sheet()
                .exec();
        return;
    }

    //FIXME this synconicity is evil, but so is asyncronicity here
    QEventLoop loop;
    connect( result, SIGNAL(parseGood( unsigned )), &loop, SLOT(quit()) );
    connect( result, SIGNAL(parseBad( int, QString, int )), &loop, SLOT(quit()) );
    loop.exec();

    LocalRadioTrackSource* source = new LocalRadioTrackSource( result );
    m_radio->play( RadioStation( tags.join( ", " ) ), source  );
    source->start();
}


void
App::onTrackSpooled( const Track& t )
{    
    m_mainwindow->setWindowTitle( t );
    
    m_mainwindow->ui.play->blockSignals( true );
    m_mainwindow->ui.play->setChecked( !t.isNull() );
    m_mainwindow->ui.play->blockSignals( false );
}


void
App::onPlayActionToggled( bool b )
{
    if (b)
        play();
    else
        m_radio->stop();
}

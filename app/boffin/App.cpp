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
#include "lib/lastfm/radio/Radio.h"
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

/// content scanner
    try
    {
        LocalContentConfig cfg;
        PickDirsDialog picker( window );
        picker.setDirs( cfg.getScanDirs() );
        if (picker.exec() == QDialog::Rejected)
            throw 1;
        cfg.setScanDirs( picker.getDirs() );
        cfg.updateVolumeAvailability();
    } 
    catch (QueryError e)
    {
        int ii = 0;
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
    m_localRql = new LocalRqlPlugin();
    m_localRql->init();

/// content resolver
    m_trackResolver = new TrackResolver();
    m_resolver = new Resolver( QList<ITrackResolverPlugin*>() << m_trackResolver );

////// scanning widget
    ScanProgressWidget* progress = new ScanProgressWidget;
    window->setCentralWidget( progress );
    connect( m_contentScanner, SIGNAL(trackScanned(Track, int, int)), progress, SLOT(onNewTrack( Track, int, int )) );
    connect( m_contentScanner, SIGNAL(finished()), SLOT(onScanningFinished()) );
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
    
    TagCloudView* view = new TagCloudView;
    view->setModel( new TagCloudModel );
    view->setItemDelegate( new TagDelegate );
    m_mainwindow->setCentralWidget( view );
    
    view->setFrameStyle( QFrame::NoFrame );

    qDebug() << "Bye!" << time.elapsed() << "ms";
}

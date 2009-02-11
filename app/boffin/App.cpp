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
#include "LocalContentScannerThread.h"
#include "LocalContentScanner.h"
#include "TrackTagUpdater.h"
#include "LocalRqlPlugin.h"
#include "TrackResolver.h"
#include "Resolver.h"
#include "lib/lastfm/radio/Radio.h"
#include <phonon/audiooutput.h>
#include <phonon/backendcapabilities.h>
#include "XspfResolvingTrackSource.h"
#include <QMenu>
#include <QVBoxLayout>

#define OUTPUT_DEVICE_KEY "OutputDevice"


App::App( int& argc, char** argv )
    :Unicorn::Application( argc, argv ), m_radio( 0 )
{
/// content resolver
    m_contentScanner = new LocalContentScanner;
    m_trackTagUpdater = TrackTagUpdater::create(
            "http://musiclookup.last.fm/trackresolve",
            100,        // number of days track tags are good 
            5);         // 5 minute delay between web requests
    connect(m_contentScanner, SIGNAL(trackScanned(Track, int, int)), m_trackTagUpdater, SLOT(needsUpdate()));

    m_contentScannerThread = new LocalContentScannerThread(m_contentScanner);
    m_contentScannerThread->start();

    m_localRql = new LocalRqlPlugin();
    m_localRql->init();

    m_trackResolver = new TrackResolver();
    m_resolver = new Resolver( QList<ITrackResolverPlugin*>() << m_trackResolver );

/// blah
    if (argc > 1) {
        openXspf( argv[1] );
    }
}


App::~App()
{
    m_contentScanner->stop();
    m_contentScannerThread->wait();
    delete m_contentScanner;
    delete m_contentScannerThread;    
    
    QSettings().setValue( OUTPUT_DEVICE_KEY, m_radio->audioOutput()->outputDevice().name() );
}


#include "ScanProgressWidget.h"
#include "ScanLocationsWidget.h"
void
App::setMainWindow( MainWindow* window )
{
    m_mainwindow = window;
    
////// audio output device
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

////// scanning widget
    ScanProgressWidget* progress = new ScanProgressWidget;
    window->setCentralWidget( progress );
    connect( m_contentScanner, SIGNAL(trackScanned(Track, int, int)), progress, SLOT(onNewTrack( Track )) );
    connect( m_contentScanner, SIGNAL(finished()), SLOT(onScanningFinished()) );

    ScanLocationsWidget* locations = new ScanLocationsWidget;
    locations->setLocations( QStringList() << "/jono/s/bad/boy/Tunes" << "/doug/s/super/Music" );
    
    QVBoxLayout* v = new QVBoxLayout( progress );
    v->addStretch();
    v->addWidget( locations );
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
    disconnect( sender(), 0, this, 0 ); //only once pls
    
    TagCloudView* view = new TagCloudView;
    view->setModel( new TagCloudModel );
    view->setItemDelegate( new TagDelegate );
    m_mainwindow->setCentralWidget( view );
}

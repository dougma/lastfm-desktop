/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
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
#include "PlayerEvent.h"
#include "PlayerListener.h"
#include "PlayerManager.h"
#include "Settings.h"
#include "version.h"
#include "mac/ITunesListener.h"
#include "radio/RadioWidget.h"
#include "widgets/DiagnosticsDialog.h"
#include "widgets/MainWindow.h"
#include "lib/core/MessageBoxBuilder.h"
#include "lib/scrobble/Scrobbler.h"
#include "lib/radio/Radio.h"
#include <QLineEdit>
#include <QSystemTrayIcon>
#include "phonon/audiooutput.h"

#ifdef WIN32
    #include "legacy/disableHelperApp.cpp"
#endif


App::App( int argc, char** argv ) 
   : Unicorn::Application( argc, argv )
{
    // IMPORTANT don't allow any GUI thread message loops to run during this
    // ctor! Things will crash in interesting ways!
    //TODO bootstrapping
    
    m_settings = new Settings( VERSION, applicationFilePath() );

    m_playerListener = new PlayerListener( this );
    connect( m_playerListener, SIGNAL(bootstrapCompleted( QString, QString )), SLOT(onBootstrapCompleted( QString, QString )) );
    
    m_playerManager = new PlayerManager( m_playerListener );
    connect( m_playerManager, SIGNAL(event( int, QVariant )), SLOT(onAppEvent( int, QVariant )) );
    
#ifdef Q_WS_MAC
    new ITunesListener( m_playerListener->port(), this );
#endif
    
    ScrobblerInit init;
    init.username = The::settings().username();
    init.sessionKey = The::settings().sessionKey();
    init.clientId = "ass";
    m_scrobbler = new Scrobbler( init );
    
	m_radio = new Radio( new Phonon::AudioOutput );
	m_radio->audioOutput()->setVolume( 0.8 ); //TODO rememeber
	connect( m_radio, SIGNAL(stateChanged( Radio::State, Radio::State )), SLOT(onRadioStateChanged( Radio::State, Radio::State )) );
    connect( m_radio, SIGNAL(trackStarted( Track )), SLOT(onRadioTrackStarted( Track )) );
	
    DiagnosticsDialog::observe( m_scrobbler );

    setQuitOnLastWindowClosed( false );

    //TODO do once?
#ifdef WIN32
    Legacy::disableHelperApp();
#endif
}


App::~App()
{
    delete m_scrobbler;
    delete Settings::instance;
}


void
App::setMainWindow( MainWindow* window )
{
    m_mainWindow = window;

    connect( window->ui.love, SIGNAL(triggered()), SLOT(love()) );
    connect( window->ui.ban,  SIGNAL(triggered()), SLOT(ban()) );
    connect( window->ui.logout, SIGNAL(triggered()), SLOT(logout()) );
	connect( window->ui.skip, SIGNAL(triggered()), m_radio, SLOT(skip()) );

	// for now not on mac, FIXME eventually, in tray as option, default off
#ifndef Q_WS_MAC
    m_trayIcon = new QSystemTrayIcon( window );
    m_trayIcon->setIcon( QPixmap(":/16x16/as.png") );
    m_trayIcon->show();

    QMenu* menu = new QMenu;
    menu->addAction( window->ui.quit );
    m_trayIcon->setContextMenu( menu );
    connect( m_trayIcon, 
             SIGNAL(activated( QSystemTrayIcon::ActivationReason )), 
             window, 
             SLOT(onSystemTrayIconActivated( QSystemTrayIcon::ActivationReason )) );
#endif
}


void
App::onAppEvent( int e, const QVariant& d )
{
	qDebug() << (PlayerEvent::Enum)e;
	
    switch (e)
    {
        case PlayerEvent::TrackStarted:
        {
            Track t = d.value<ObservedTrack>();
            m_scrobbler->nowPlaying( t );

            // no tooltips on mac
        #ifndef Q_WS_MAC
            m_trayIcon->setToolTip( t.prettyTitle() );
        #endif
            break;
        }

		case PlayerEvent::TrackEnded:
            m_scrobbler->submit();
            break;
			
        case PlayerEvent::ScrobblePointReached:
            m_scrobbler->cache( d.value<ObservedTrack>() );
            break;
    }

    emit event( e, d );
}


void
App::onWsError( Ws::Error e )
{
    switch (e)
    {
        case Ws::OperationFailed:
            //TODOCOPY
            //TODO use the non intrusive status messages
            MessageBoxBuilder( m_mainWindow )
                    .setTitle( "Oops" )
                    .setText( "Last.fm is b0rked" )
                    .exec();
            break;

        case Ws::InvalidSessionKey:
            logout();
            break;
			
		default:
			break;
    }
}


void 
App::onScrobblerStatusChanged( int e )
{
    // the scrobbler will not function for this session, the user will need to
    // restart in order to scrobble - after fixing the relevent issue!

    switch (e)
    {
        case Scrobbler::ErrorBannedClient:
            MessageBoxBuilder( m_mainWindow )
                .setIcon( QMessageBox::Warning )
                .setTitle( tr("Upgrade Required") )
                .setText( tr("Scrobbling will not work because this software is too old.") )
                .exec();
            break;

        case Scrobbler::ErrorInvalidSessionKey:
            logout();
            break;

        case Scrobbler::ErrorBadTime:
            MessageBoxBuilder( m_mainWindow )
                .setIcon( QMessageBox::Warning )
                .setTitle( tr("Incorrect Time") )
                .setText( tr("<p>Last.fm cannot authorise any scrobbling! :("
                             "<p>It appears your computer disagrees with us about what the time is."
                             "<p>If you are sure the time is right, check the <b>date</b> is correct and check your "
                             "<b>timezone</b> is not set to something miles away, like Mars." 
                             "<p>We're sorry about this restriction, but we impose it to help prevent "
                                "scrobble spamming.") )
                .exec();
            break;
    }
}


void
App::onRadioStateChanged( Radio::State oldstate, Radio::State newstate )
{
	qDebug() << newstate << "but was:" << oldstate;
	
	switch (newstate)
	{
		case Radio::Playing:
			if (oldstate == Radio::Rebuffering)
				m_playerManager->onPlaybackResumed( "ass" );
			break;
			
		case Radio::Stopped:
		    m_playerManager->onPlaybackEnded( "ass" );
			break;
			
		case Radio::TuningIn:
			if (oldstate == Radio::Stopped)
				m_playerManager->onPlayerConnected( "ass" );
			m_playerManager->onTrackEnded( "ass" );
			onAppEvent( PlayerEvent::TuningIn, QVariant::fromValue( m_radio->station() ) );
			break;
			
		case Radio::Rebuffering:
			m_playerManager->onPlaybackPaused( "ass" );
			break;
	}
}


void 
App::onBootstrapCompleted( const QString& playerId, const QString& username )
{}


void
App::love()
{
	Track t = track();
	MutableTrack( t ).upgradeRating( Track::Loved );
	MessageBoxBuilder( m_mainWindow ).setText( "We need to set the love button to look loved or some other feedback!" ).exec();
	t.love();
}


void
App::ban()
{
	Track t = track();
	MutableTrack( t ).upgradeRating( Track::Banned );
	m_radio->skip();
	t.ban();
}
 

void
App::logout()
{
    logoutAtQuit();
    quit(); //TODO warn the user at least first! and restart as well
}


void
App::open( const QUrl& url )
{
    m_radio->play( RadioStation( url.toString() ) );
}


void
App::onRadioTrackStarted( const Track& t )
{
	MutableTrack( t ).setPlayerId( "ass" );
	m_playerManager->onTrackStarted( t );
}


Track
App::track() const
{
    return m_playerManager->track();
}


namespace The
{
    Radio& radio() { return *app().m_radio; }
    MainWindow& mainWindow() { return *app().m_mainWindow; }
    Settings& settings() { return *app().m_settings; }
}

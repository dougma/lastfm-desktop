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
#include "PlayerListener.h"
#include "PlayerManager.h"
#include "Settings.h"
#include "version.h"
#include "mac/ITunesListener.h"
#include "radio/RadioWidget.h"
#include "widgets/DiagnosticsDialog.h"
#include "widgets/MainWindow.h"
#include "lib/core/QMessageBoxBuilder.h"
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

    PlayerListener* listener = new PlayerListener( this );
    connect( listener, SIGNAL(bootstrapCompleted( QString )), SLOT(onBootstrapCompleted( QString )) );
    
    m_playerManager = new PlayerManager( listener );
    connect( m_playerManager, SIGNAL(playerChanged( QString )), SIGNAL(playerChanged( QString )) );
    connect( m_playerManager, SIGNAL(stateChanged( State, Track )), SIGNAL(stateChanged( State, Track )) );
    connect( m_playerManager, SIGNAL(stopped()), SIGNAL(stopped()) );
    connect( m_playerManager, SIGNAL(trackSpooled( Track, StopWatch* )), SIGNAL(trackSpooled( Track, StopWatch* )) );
    connect( m_playerManager, SIGNAL(trackUnspooled( Track )), SIGNAL(trackUnspooled( Track )) );
    connect( m_playerManager, SIGNAL(scrobblePointReached( Track )), SIGNAL(scrobblePointReached( Track )) ); 
    
#ifdef Q_WS_MAC
    new ITunesListener( listener->port(), this );
#endif
    
    m_scrobbler = new Scrobbler( "ass" );
    
    connect( m_playerManager, SIGNAL(trackSpooled( Track )), m_scrobbler, SLOT(nowPlaying( Track )) );
    connect( m_playerManager, SIGNAL(trackUnspooled( Track )), m_scrobbler, SLOT(submit()) );
    connect( m_playerManager, SIGNAL(scrobblePointReached( Track )), m_scrobbler, SLOT(cache( Track )) );
    
	m_radio = new Radio( new Phonon::AudioOutput );
	m_radio->audioOutput()->setVolume( 0.8 ); //TODO rememeber

	connect( m_radio, SIGNAL(tuningIn( RadioStation )), m_playerManager, SLOT(onRadioTuningIn( RadioStation )) );
    connect( m_radio, SIGNAL(trackSpooled( Track )), m_playerManager, SLOT(onRadioTrackSpooled( Track )) );
    connect( m_radio, SIGNAL(trackStarted( Track )), m_playerManager, SLOT(onRadioTrackStarted( Track )) );
    connect( m_radio, SIGNAL(stopped()), m_playerManager, SLOT(onRadioStopped()) );
    
    DiagnosticsDialog::observe( m_scrobbler );

    setQuitOnLastWindowClosed( false );
	
    //TODO do once?
#ifdef WIN32
    Legacy::disableHelperApp();
#endif

    qDebug() << argv[0];

    parseArguments( arguments() );
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

    connect( window->ui.love, SIGNAL(triggered( bool )), SLOT(love( bool )) );
    connect( window->ui.ban,  SIGNAL(triggered()), SLOT(ban()) );
    connect( window->ui.logout, SIGNAL(triggered()), SLOT(logout()) );
	connect( window->ui.skip, SIGNAL(triggered()), m_radio, SLOT(skip()) );

	// for now not on mac, FIXME eventually, in tray as option, default off
#ifndef Q_WS_MAC
    m_trayIcon = new QSystemTrayIcon( window );
    m_trayIcon->setIcon( QPixmap(":/16x16/as.png") );
    m_trayIcon->show();

    QMenu* menu = new QMenu;
	menu->addAction( tr( "Open" ), window, SLOT(show()) );
    menu->addAction( window->ui.quit );
    m_trayIcon->setContextMenu( menu );
    connect( m_trayIcon, 
             SIGNAL(activated( QSystemTrayIcon::ActivationReason )), 
             window, 
             SLOT(onSystemTrayIconActivated( QSystemTrayIcon::ActivationReason )) );
#endif
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
App::onBootstrapCompleted( const QString& playerId )
{}


void
App::love( bool b )
{
	Track t = m_playerManager->track();

	if( b )
	{
		MutableTrack( t ).upgradeRating( Track::Loved );
		t.love();
	}
	else
	{
		MutableTrack( t ).downgradeRating( Track::Loved );
		//Need t.unlove() but waiting on webservice!
	}
}


void
App::ban()
{
	Track t = m_playerManager->track();
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


namespace  //anonymous namespace, keep to this file
{
    enum Argument
    {
        LastFmUrl,
        Pause, //toggles pause
        Skip,
        Unknown
    };
    
    Argument argument( const QString& arg )
    {
        if (arg == "--pause") return Pause;
        if (arg == "--skip") return Skip;
        
        QUrl url( arg );
        if (url.isValid() && url.scheme() == "lastfm") return LastFmUrl;

        return Unknown;
    }
}


void
App::parseArguments( const QStringList& args )
{  
    if (args.size() == 1)
        return;
    
    qDebug() << args;
    
    foreach (QString const arg, args.mid( 1 ))
        switch (argument( arg ))
        {
            case LastFmUrl:
                open( QUrl( arg ) );
                break;
                
            case Skip:
            case Pause:
                qDebug() << "Unimplemented:" << arg;
                break;
                
            case Unknown:
                qDebug() << "Unknown argument:" << arg;
                break;
        }
}


namespace The
{
    Radio& radio() { return *app().m_radio; }
    MainWindow& mainWindow() { return *app().m_mainWindow; }
    Settings& settings() { return *app().m_settings; }
}

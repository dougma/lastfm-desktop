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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "App.h"
#include "_version.h"
#include "container.h"
#include "UglySettings.h"
#include "lib/listener/PlayerMediator.h"
#include "lib/listener/PlayerListener.h"
#include "lib/listener/legacy/LegacyPlayerListener.h"
#include "lib/unicorn/QMessageBoxBuilder.h"
#include "lib/lastfm/fingerprint/Fingerprint.h"
#include "lib/lastfm/scrobble/Scrobbler.h"
#include "lib/lastfm/radio/Radio.h"
#include "lib/lastfm/radio/Tuner.h"
#include "lib/lastfm/radio/LegacyTuner.h"
#include "lib/lastfm/ws/WsConnectionMonitor.h"
#include "lib/lastfm/ws/WsReply.h"
#include <QLineEdit>
#include <QSystemTrayIcon>
#include <QThreadPool>
#include <phonon/audiooutput.h>
#include <phonon/backendcapabilities.h>

//client 2
#include "ExtractIdentifiersJob.h"
#include "mac/ITunesPluginInstaller.h"
#include "StateMachine.h"
#include "app/client/Settings.h"

#ifdef __APPLE__
    extern void qt_mac_set_menubar_icons( bool );    
    #include "lib/listener/mac/ITunesListener.h"
#endif


App::App( int& argc, char** argv ) 
   : Unicorn::Application( argc, argv )
   , machine( 0 )
   , radio( 0 )
   , scrobbler( 0 )
{
#ifdef Q_WS_MAC
    qt_mac_set_menubar_icons( false );
#endif
    
    // ATTENTION! Under no circumstance change these strings! --mxcl
#ifdef WIN32
    Ws::UserAgent = "Last.fm Client " VERSION " (Windows)";
#elif defined (Q_WS_MAC)
    Ws::UserAgent = "Last.fm Client " VERSION " (OS X)";
#elif defined (Q_WS_X11)
    Ws::UserAgent = "Last.fm Client " VERSION " (X11)";
#endif

    
	The::settings().setVersion( applicationVersion() );
#ifdef NDEBUG
    The::settings().setPath( applicationFilePath() );
#endif

    machine = new StateMachine( this );
//TODO    machine->setScrobbleFraction( qreal(moose::Settings().scrobblePoint()) / 100 );    
    connect( machine, SIGNAL(playerChanged( QString )), SIGNAL(playerChanged( QString )) );
    connect( machine, SIGNAL(stateChanged( State, Track )), SIGNAL(stateChanged( State, Track )) );
    connect( machine, SIGNAL(stopped()), SIGNAL(stopped()) );
    connect( machine, SIGNAL(trackSpooled( Track, StopWatch* )), SIGNAL(trackSpooled( Track, StopWatch* )) );
    connect( machine, SIGNAL(trackUnspooled( Track )), SIGNAL(trackUnspooled( Track )) );
    connect( machine, SIGNAL(scrobblePointReached( Track )), SIGNAL(scrobblePointReached( Track )) ); 

    connect( machine, SIGNAL(trackSpooled( Track )), SLOT(onTrackSpooled( Track )) );
    connect( machine, SIGNAL(trackUnspooled( Track )), SLOT(onTrackUnspooled( Track )) );
    
    PlayerMediator* mediator = new PlayerMediator( this );
    connect( mediator, SIGNAL(activeConnectionChanged( PlayerConnection* )), machine, SLOT(setConnection( PlayerConnection* )) );
    connect( new LegacyPlayerListener( this ), SIGNAL(newConnection( PlayerConnection* )), mediator, SLOT(follow( PlayerConnection* )) );
#ifdef __APPLE__
    ITunesListener* itunes_listener = new ITunesListener( this );
    connect( itunes_listener, SIGNAL(newConnection( PlayerConnection* )), mediator, SLOT(follow( PlayerConnection* )) );
    itunes_listener->start();
#else

    PlayerListener* listener = 0;
    try
    {
        listener = new PlayerListener( this );
        connect( m_listener, SIGNAL(newConnection( PlayerConnection* )), mediator, SLOT(follow( PlayerConnection* )) );
    }
    catch (PlayerListener::SocketFailure& e)
    {
        qWarning() << e.what();
    }
#endif

    scrobbler = new Scrobbler( "ass" ); //connections happen later

////// radio
    Phonon::AudioOutput* audioOutput = new Phonon::AudioOutput( Phonon::MusicCategory, this );
	audioOutput->setVolume( The::settings().volume() / 100 );

    QString audioOutputDeviceName = moose::Settings().audioOutputDeviceName();
    if (audioOutputDeviceName.size())
    {
        foreach (Phonon::AudioOutputDevice d, Phonon::BackendCapabilities::availableAudioOutputDevices())
            if (d.name() == audioOutputDeviceName) {
                audioOutput->setOutputDevice( d );
                break;
            }
    }

	radio = new Radio( audioOutput );
	connect( radio, SIGNAL(tuningIn( RadioStation )), machine, SLOT(onRadioTuningIn( RadioStation )) );
    connect( radio, SIGNAL(trackSpooled( Track )), machine, SLOT(onRadioTrackSpooled( Track )) );
    connect( radio, SIGNAL(trackStarted( Track )), machine, SLOT(onRadioTrackStarted( Track )) );
    connect( radio, SIGNAL(stopped()), machine, SLOT(onRadioStopped()) );
    connect( radio, SIGNAL(error( int, QVariant )), SLOT(onRadioError( int, QVariant )) );
    
#ifdef __APPLE__
#ifdef NDEBUG
    ITunesPluginInstaller pluginInstaller;
    pluginInstaller.install();
#endif
#endif

    setQuitOnLastWindowClosed( false );

#ifdef WIN32
    //TODO do only once?
    Legacy::disableHelperApp();
#endif
    
    connect(AuthenticatedUser().getInfo(),
            SIGNAL(finished( WsReply* )),
            SLOT(onUserGotInfo( WsReply* )));
}


App::~App()
{
    The::settings().setVolume( radio->audioOutput()->volume() );
    
    delete scrobbler;
    delete radio;
}



void
App::setContainer( Container* window )
{
    container = window;
    
//TODO    window->ui.scrobble->setChecked( false ); // if true we set it up the bomb later in this function

//TODO    connect( window->ui.love, SIGNAL(triggered( bool )), SLOT(love( bool )) );
//TODO    connect( window->ui.ban,  SIGNAL(triggered()), SLOT(ban()) );
//TODO    connect( window->ui.logout, SIGNAL(triggered()), SLOT(logout()) );
//TODO	connect( window->ui.skip, SIGNAL(triggered()), radio, SLOT(skip()) );
//TODO    connect( window->ui.scrobble, SIGNAL(toggled( bool )), SLOT(setScrobblingEnabled( bool )) );

	// for now not on mac, FIXME eventually, in tray as option, default off
#ifndef Q_WS_MAC
    trayicon = new QSystemTrayIcon( window );
    trayicon->setIcon( QPixmap(":/16x16.png") );
    trayicon->show();

    QMenu* menu = new QMenu;
	menu->addAction( tr( "Open" ), window, SLOT(show()) );
    menu->addAction( window->ui.quit );
    trayicon->setContextMenu( menu );
    connect( trayicon, 
             SIGNAL(activated( QSystemTrayIcon::ActivationReason )), 
             window, 
             SLOT(onSystemTrayIconActivated( QSystemTrayIcon::ActivationReason )) );
#endif

    if (The::currentUser().isLogToProfile())
    {
        // only do if true, otherwise the state machine will be fucked
        setScrobblingEnabled( true );
    }

//TODO    connect( scrobbler, SIGNAL(status( int )), window->ui.diagnostics, SLOT(scrobbleActivity( int )) );

    connect( container, SIGNAL(play( QUrl )), SLOT(open( QUrl )) );
    connect( container, SIGNAL(skip()), radio, SLOT(skip()) );
    connect( container, SIGNAL(stop()), radio, SLOT(stop()) );
}


void
App::setScrobblingEnabled( bool b )
{
    (void) b;
    
    //TODO
#if 0
    if (sender() != container->ui.scrobble)
    {
        Q_ASSERT( container->ui.scrobble->isChecked() != b );
        container->ui.scrobble->setChecked( b ); //recursive
    }
    else if (b)
    {
        connect( machine, SIGNAL(trackSpooled( Track )), scrobbler, SLOT(nowPlaying( Track )) );
        connect( machine, SIGNAL(scrobblePointReached( Track )), scrobbler, SLOT(cache( Track )) );
        
        connect( new WsConnectionMonitor( scrobbler ), SIGNAL(up()), scrobbler, SLOT(rehandshake()) );
        
        Track t = machine->track();
        if (!t.isNull())
            scrobbler->nowPlaying( t );
    }
    else {
        disconnect( machine, 0, scrobbler, 0 );
        delete scrobbler->findChild<WsConnectionMonitor*>();
    }
    
    The::settings().setLogToProfile( b );
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
            MessageBoxBuilder( container )
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
App::onRadioError( int code, const QVariant& data )
{
    switch (code)
    {
        case Ws::NotEnoughContent:
//TODO            emit error( tr("Sorry, there is no more content available for this radio station.") );
            break;
            
        case Ws::MalformedResponse:
        case Ws::TryAgainLater:
//TODO            emit error( tr("Sorry, there was a radio error at Last.fm. Please try again later.") );
            break;
            
        case Ws::SubscribersOnly:
//TODO            emit error( tr("Sorry, this station is only available to Last.fm subscribers. "
//                           "<A href='http://www.last.fm/subscribe'>Sign up</a>.") );
            break;

        case Ws::UnknownError:
            // string contains Phonon generated user readable error message
//TODO            emit error( data.toString() );
            break;

        default:
//TODO            emit error( tr("Sorry, an unexpected error occurred.") );
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
        case Scrobbler::ErrorBannedClientVersion:
            MessageBoxBuilder( container )
                .setIcon( QMessageBox::Warning )
                .setTitle( tr("Upgrade Required") )
                .setText( tr("Scrobbling will not work because this software is too old.") )
                .exec();
            break;

        case Scrobbler::ErrorInvalidSessionKey:
            logout();
            break;

        case Scrobbler::ErrorBadTime:
            MessageBoxBuilder( container )
                .setIcon( QMessageBox::Warning )
                .setTitle( tr("Incorrect Time") )
                .setText( tr("<p>Last.fm cannot authorise any scrobbling! :("
                             "<p>It appears your computer disagrees with us about what the time is."
                             "<p>If you are sure the time is right, check the <b>date</b> is correct and check your "
                             "<b>timezone</b> is not set to something miles away, like Mars.") )
                .exec();
            break;
    }
}


void
App::onUserGotInfo( WsReply* reply )
{
    try
    {
    #ifdef WIN32
        if (reply->lfm()["user"]["bootstrap"].text() != "0")
        {
//TODO            BootstrapDialog( m_listener, container ).exec();
        }
    #endif
    }
    catch (CoreDomElement::Exception& e)
    {
        qWarning() << e;
    }
} 


void
App::onTrackSpooled( const Track& t )
{
    if (t.isNull()) return;
    
    qDebug() << t;
    
#ifdef Q_OS_MAC
    if (t.source() == Track::Player && t.isMp3() && The::currentUser().fingerprintingEnabled())
    {
        FingerprintId fpid = Fingerprint( t ).id();
        
        if (t.mbid().isNull() || fpid.isNull())
        {
            ExtractIdentifiersJob* job = new ExtractIdentifiersJob( t );
            QThreadPool::globalInstance()->start( job );
            
//TODO            connect( job, SIGNAL(fingerprinted( Track )), container->ui.diagnostics, SLOT(fingerprinted( Track )) );
        }
    }
#endif
    
}


void
App::onTrackUnspooled( const Track& t )
{
    //TODO
#if 0
    Scrobble s( t );
    bool const enabled = container->ui.scrobble->isChecked();
    
    // always submit loved tracks, scrobble point reached or not
    // always submit banned tracks, scrobble point reached or not
    // only submit skipped tracks, scrobble point reached or not, but only if scrobbling is on

    if (s.isLoved() || s.isBanned() || s.isSkipped() && enabled && s.source() == Track::LastFmRadio)
    {
        scrobbler->cache( t );
        scrobbler->submit();
    }
    else if (enabled)
        scrobbler->submit();
#endif
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
    qDebug() << url;
    
    RadioStation station( url.toString() );

    //TODO station object should return Tuner type
    AbstractTrackSource* source = station.isLegacyPlaylist()
            ? (AbstractTrackSource*) new LegacyTuner( station, CoreSettings().value( "Password" ).toString() )
            : (AbstractTrackSource*) new Tuner( station );

    radio->play( station, source );
}


namespace //anonymous namespace, keep to this file
{
    enum Argument
    {
        LastFmUrl,
        Pause, //toggles pause
        Skip,
        IPodDetected,
		Twiddling,
        Twiddled,
        Unknown
    };
    
    Argument argument( const QString& arg )
    {
        if (arg == "--pause") return Pause;
        if (arg == "--skip") return Skip;
        if (arg == "--ipod-detected") return IPodDetected;
        if (arg == "--twiddling") return Twiddling;
		if (arg == "--twiddled") return Twiddled;
        
        QUrl url( arg );
        if (url.isValid() && url.scheme() == "lastfm") return LastFmUrl;

        return Unknown;
    }
}


void
App::parseArguments( const QStringList& args )
{
    qDebug() << args;	
	
    if (args.size() == 1)
        return;
    
    foreach (QString const arg, args.mid( 1 ))
        switch (argument( arg ))
        {
            case IPodDetected:
//TODO                emit status( tr("Your iPod will be scrobbled to your Last.fm profile from now on."), "" );
                return;

			case Twiddling:
//TODO                emit status( tr("Your iPod scrobbles are being determined."), "twiddling" );
				return;

			case Twiddled:
            {	
//TODO                emit status( tr("iPod scrobbling complete."), "twiddling" ); // clear status for ipod scrobbling
//TODO                emit status( "", "twiddling" ); // removes message after small delay
//TODO                submitTwiddleCache( args.value( 2 ) );
				return;
            }

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

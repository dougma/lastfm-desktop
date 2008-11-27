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
#include "Settings.h"
#include "Resolver.h"
#include "PluginHost.h"
#include "bootstrap/BootstrapDialog.h"
#include "XspfResolvingTrackSource.h"
#include "ExtractIdentifiersJob.h"
#include "ipod/BatchScrobbleDialog.h"
#include "ipod/IPodScrobbleCache.h"
#include "mac/MacStyle.h"
#include "mac/ITunesListener.h"
#include "mac/ITunesPluginInstaller.h"
#include "player/PlayerListener.h"
#include "player/PlayerMediator.h"
#include "widgets/DiagnosticsDialog.h"
#include "widgets/MainWindow.h"
#include "app/twiddly/IPodScrobble.h"
#include "lib/unicorn/QMessageBoxBuilder.h"
#include "lib/lastfm/fingerprint/Fingerprint.h"
#include "lib/lastfm/scrobble/Scrobbler.h"
#include "lib/lastfm/radio/Radio.h"
#include "lib/lastfm/radio/Tuner.h"
#include "lib/lastfm/radio/LegacyTuner.h"
#include "lib/lastfm/ws/WsReply.h"
#include <QLineEdit>
#include <QSystemTrayIcon>
#include <QThreadPool>
#include <phonon/audiooutput.h>

#ifdef WIN32
    #include "legacy/disableHelperApp.cpp"
#endif
#ifdef Q_WS_MAC
    extern void qt_mac_set_menubar_icons( bool );
#endif



App::App( int& argc, char** argv ) 
   : Unicorn::Application( argc, argv ), m_scrobbler( 0 ), m_radio( 0 ), m_resolver( 0 ), m_listener( 0 )
{
#ifdef Q_WS_MAC
    // I have to set it on the whole application as QMainWindow grabs some
    // pixelmetrics when it is created, which means a post setStyle doesn't
    // work
    setStyle( new UnicornMacStyle );
    qt_mac_set_menubar_icons( false );
#endif
    
    // ATTENTION! Under no circumstance change these strings! --mxcl
#ifdef WIN32
    Ws::UserAgent = "Last.fm Client (Windows)";
#elif defined (Q_WS_MAC)
    Ws::UserAgent = "Last.fm Client (OS X)";
#elif defined (Q_WS_X11)
    Ws::UserAgent = "Last.fm Client (X11)";
#endif
   
	QSettings s;
    bool updgradeJustOccurred = applicationVersion() != s.value( "Version", "An Impossible Version String" );
	s.setValue( "Version", applicationVersion() );
#ifdef NDEBUG
    s.setValue( "Path", applicationFilePath() );
#endif
    
    try {
        m_listener = new PlayerListener( this );
        connect( m_listener, SIGNAL(bootstrapCompleted( QString )), SLOT(onBootstrapCompleted( QString )) );
    }
    catch (PlayerListener::SocketFailure& e)
    {
        MessageBoxBuilder( 0 )
                .setTitle( "Sorry" )
                .setText( "You can't run the old client and the new client at once yet! We'll stay open, but scrobbling won't work." )
                .exec();
    }
    
    m_playerMediator = new PlayerMediator( m_listener );
    connect( m_playerMediator, SIGNAL(playerChanged( QString )), SIGNAL(playerChanged( QString )) );
    connect( m_playerMediator, SIGNAL(stateChanged( State, Track )), SIGNAL(stateChanged( State, Track )) );
    connect( m_playerMediator, SIGNAL(stopped()), SIGNAL(stopped()) );
    connect( m_playerMediator, SIGNAL(trackSpooled( Track, StopWatch* )), SIGNAL(trackSpooled( Track, StopWatch* )) );
    connect( m_playerMediator, SIGNAL(trackUnspooled( Track )), SIGNAL(trackUnspooled( Track )) );
    connect( m_playerMediator, SIGNAL(scrobblePointReached( Track )), SIGNAL(scrobblePointReached( Track )) ); 

    connect( m_playerMediator, SIGNAL(trackSpooled( Track )), SLOT(onTrackSpooled( Track )) );
    
#ifdef Q_WS_MAC
    new ITunesListener( m_listener->port(), this );
#endif
    
    m_scrobbler = new Scrobbler( "ass" );
    connect( m_playerMediator, SIGNAL(trackSpooled( Track )), m_scrobbler, SLOT(nowPlaying( Track )) );
    connect( m_playerMediator, SIGNAL(trackUnspooled( Track )), m_scrobbler, SLOT(submit()) );
    connect( m_playerMediator, SIGNAL(scrobblePointReached( Track )), m_scrobbler, SLOT(cache( Track )) );

#ifndef NDEBUG
    QString plugins_path = qApp->applicationDirPath();
#else
#ifdef Q_WS_X11
    QString plugins_path = "/usr/lib/lastfm/";
#else
    QString plugins_path = qApp->applicationDirPath() + "/plugins";
#endif
#endif

    PluginHost pluginHost( plugins_path );    // todo: make this a member so we can reuse it
    m_resolver = new Resolver( pluginHost.getPlugins<ITrackResolverPlugin>("TrackResolver") );
	m_radio = new Radio( new Phonon::AudioOutput );
	m_radio->audioOutput()->setVolume( 0.8 ); //TODO rememeber

	connect( m_radio, SIGNAL(tuningIn( RadioStation )), m_playerMediator, SLOT(onRadioTuningIn( RadioStation )) );
    connect( m_radio, SIGNAL(trackSpooled( Track )), m_playerMediator, SLOT(onRadioTrackSpooled( Track )) );
    connect( m_radio, SIGNAL(trackStarted( Track )), m_playerMediator, SLOT(onRadioTrackStarted( Track )) );
    connect( m_radio, SIGNAL(stopped()), m_playerMediator, SLOT(onRadioStopped()) );
    connect( m_radio, SIGNAL(error( int, QVariant )), SLOT(onRadioError( int, QVariant )) );
    
#ifdef __APPLE__
#ifdef NDEBUG
    ITunesPluginInstaller pluginInstaller;
    pluginInstaller.install();
#endif
#endif
    
    DiagnosticsDialog::observe( m_scrobbler );

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
    delete m_scrobbler;
    delete m_radio;
    delete m_resolver;
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
    m_trayIcon->setIcon( QPixmap(":/16x16.png") );
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
App::onRadioError( int code, const QVariant& data )
{
    switch (code)
    {
        case Ws::NotEnoughContent:
            emit error( tr("Sorry, there is no more content available for this radio station.") );
            break;
            
        case Ws::MalformedResponse:
        case Ws::TryAgainLater:
            emit error( tr("Sorry, there was a radio error at Last.fm. Please try again later.") );
            break;
            
        case Ws::SubscribersOnly:
            emit error( tr("Sorry, this station is only available to Last.fm subscribers. "
                           "<A href='http://www.last.fm/subscribe'>Sign up</a>.") );
            break;

        case Ws::UnknownError:
            // string contains Phonon generated user readable error message
            emit error( data.toString() );
            break;

        default:
            emit error( tr("Sorry, an unexpected error occurred.") );
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
        if (reply->lfm()["user"]["bootstrap"].text() != "0")
        {
            BootstrapDialog( m_listener, m_mainWindow ).exec();
        }
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
    
#ifdef Q_OS_MAC
    if (t.source() == Track::Player && t.isMp3() && moose::Settings().fingerprintingEnabled())
    {
        FingerprintId fpid = Fingerprint( t ).id();
        
        if (t.mbid().isNull() || fpid.isNull())
        {
            QThreadPool::globalInstance()->start( new ExtractIdentifiersJob( t ) );
        }
    }
#endif
    
}


void
App::love( bool b )
{
	MutableTrack t = m_playerMediator->track();

	if (b)
		t.love();
	else
		t.unlove();
}


void
App::ban()
{
	MutableTrack t = m_playerMediator->track();
	t.ban();
	m_radio->skip();
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
    open( RadioStation( url.toString() ) );
}

void
App::open( const RadioStation& station )
{
    m_radio->play( 
        station, 
        station.isLegacyPlaylist()
                ? (AbstractTrackSource*) new LegacyTuner( station, CoreSettings().value( "Password" ).toString() )
                : (AbstractTrackSource*) new Tuner( station ) );
}

void
App::openXspf( const QUrl& url )
{
    XspfResolvingTrackSource* src = new XspfResolvingTrackSource( m_resolver, url );
    m_radio->play( 
        RadioStation( "XSPF" ), 
        src );
    src->start();
}

// todo: change the param to be the rql string
void
App::openLocalContent( class AbstractTrackSource* trackSource )
{
    m_radio->play( RadioStation( "Local Content" ), trackSource );
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
                emit status( tr("Your iPod will be scrobbled to your Last.fm profile from now on."), "" );
                return;

			case Twiddling:
                emit status( tr("Your iPod scrobbles are being determined."), "twiddling" );
				return;

			case Twiddled:
            {
                emit status( tr("iPod scrobbling complete."), "twiddling" ); // clear status for ipod scrobbling
                emit status( "", "twiddling" ); // removes message after small delay

                IPodScrobbleCache cache( args.value( 2 ) );
                QList<Track> tracks = cache.tracks();

                if (tracks.isEmpty())
                    qWarning() << "Empty iPod scrobble cache, but that shouldn't be possible!";

                if (cache.insane() || moose::Settings().alwaysConfirmIPodScrobbles())
                {
                    BatchScrobbleDialog d( m_mainWindow );
                    d.setTracks( tracks );
                    d.exec();
                    
                    tracks = d.tracks();
                }
                
                // the scrobbler doesn't understand the playcount parameter
                // this results in a series of scrobbles with identical timestamps
                // but well. We can't do anything good about that.
                QMutableListIterator<Track> i( tracks );
                while (i.hasNext())
                {
                    IPodScrobble s = i.next();
					int const n = s.playCount();
					MutableTrack( s ).removeExtra( "playCount" );
                    for (int y = 1; y < n; --y)
                        i.insert( s );
                }

                m_scrobbler->cache( tracks );
                cache.remove(); //ie. filesystem delete

                //TODO message "Your iPod tracks will be submitted at the end of this track"
                
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

#include "the/app.h"
namespace The
{
    Radio& radio() { return *app().m_radio; }
    MainWindow& mainWindow() { return *app().m_mainWindow; }
}

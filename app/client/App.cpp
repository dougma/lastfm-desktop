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
#include "Settings.h"
#include "Resolver.h"
#include "LocalRql.h"
#include "PluginHost.h"
#include "bootstrap/BootstrapDialog.h"
#include "XspfResolvingTrackSource.h"
#include "ExtractIdentifiersJob.h"
#include "ipod/BatchScrobbleDialog.h"
#include "ipod/IPodScrobbleCache.h"
#include "mac/ITunesPluginInstaller.h"
#include "StateMachine.h"
#include "widgets/DiagnosticsDialog.h"
#include "widgets/MainWindow.h"
#include "app/twiddly/IPodScrobble.h"
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

#ifdef WIN32
    #include "win/disableLegacyHelperApp.h"
#endif
#ifdef __APPLE__
    extern void qt_mac_set_menubar_icons( bool );    
    #include "lib/listener/mac/ITunesListener.h"
#endif
#ifdef NPLUGINS
    #include "app/clientplugins/localresolver/LocalRqlPlugin.h"
    #include "app/clientplugins/localresolver/TrackResolver.h"
#endif


#include "RqlStationTemplate.h"


struct AppSettings
{    
    bool isScrobblingEnabled() const { return unicorn::UserSettings().value( "ScrobblingEnabled", true ).toBool(); }
    void setScrobblingEnabled( bool b ) { unicorn::UserSettings().setValue( "ScrobblingEnabled", b ); }
    
    uint volume() const { return QSettings().value( "Volume", 80 ).toUInt(); }
    void setVolume( uint i ) { QSettings().value( "Volume", i ); }
};


App::App( int& argc, char** argv ) throw( StubbornUserException )
   : unicorn::Application( argc, argv ), m_scrobbler( 0 ), m_radio( 0 ), m_resolver( 0 ), m_stateMachine( 0 )
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

    
	QSettings s;
    bool const upgradeJustOccurred = applicationVersion() != s.value( "Version", "An Impossible Version String" );
	s.setValue( "Version", applicationVersion() );
#ifdef NDEBUG
    s.setValue( "Path", applicationFilePath() );
#endif

    m_stateMachine = new StateMachine( this );
    m_stateMachine->setScrobbleFraction( qreal(moose::Settings().scrobblePoint()) / 100 );    
    connect( m_stateMachine, SIGNAL(playerChanged( QString )), SIGNAL(playerChanged( QString )) );
    connect( m_stateMachine, SIGNAL(stateChanged( State, Track )), SIGNAL(stateChanged( State, Track )) );
    connect( m_stateMachine, SIGNAL(stopped()), SIGNAL(stopped()) );
    connect( m_stateMachine, SIGNAL(trackSpooled( Track, StopWatch* )), SIGNAL(trackSpooled( Track, StopWatch* )) );
    connect( m_stateMachine, SIGNAL(trackUnspooled( Track )), SIGNAL(trackUnspooled( Track )) );
    connect( m_stateMachine, SIGNAL(scrobblePointReached( Track )), SIGNAL(scrobblePointReached( Track )) ); 

    connect( m_stateMachine, SIGNAL(trackSpooled( Track )), SLOT(onTrackSpooled( Track )) );
    connect( m_stateMachine, SIGNAL(trackUnspooled( Track )), SLOT(onTrackUnspooled( Track )) );
    
    PlayerMediator* mediator = new PlayerMediator( this );
    connect( mediator, SIGNAL(activeConnectionChanged( PlayerConnection* )), m_stateMachine, SLOT(setConnection( PlayerConnection* )) );
    connect( new LegacyPlayerListener( this ), SIGNAL(newConnection( PlayerConnection* )), mediator, SLOT(follow( PlayerConnection* )) );
#ifdef __APPLE__
    ITunesListener* itunes_listener = new ITunesListener( this );
    connect( itunes_listener, SIGNAL(newConnection( PlayerConnection* )), mediator, SLOT(follow( PlayerConnection* )) );
    itunes_listener->start();
#else
    try
    {
        m_listener = new PlayerListener( this );
        connect( m_listener, SIGNAL(newConnection( PlayerConnection* )), mediator, SLOT(follow( PlayerConnection* )) );
    }
    catch (std::runtime_error& e)
    {
        m_listener = 0;
        qWarning() << e.what();
        //TODO diagnostics dialog warning
    }
#endif

    m_scrobbler = new Scrobbler( "ass" ); //connections happen later

#ifndef NDEBUG
    QString plugins_path = qApp->applicationDirPath();
#else
#ifdef Q_WS_X11
    QString plugins_path = "/usr/lib/lastfm/";
#else
    QString plugins_path = qApp->applicationDirPath() + "/plugins";
#endif
#endif

#ifdef NPLUGINS 
    m_localRql = new LocalRql( QList<ILocalRqlPlugin*>() << new LocalRqlPlugin );
    m_resolver = new Resolver( QList<ITrackResolverPlugin*>() << new TrackResolver );
#else
    PluginHost pluginHost( plugins_path );    // todo: make this a member so we can reuse it?
    m_localRql = new LocalRql( pluginHost.getPlugins<ILocalRqlPlugin>("LocalRql") );
    m_resolver = new Resolver( pluginHost.getPlugins<ITrackResolverPlugin>("TrackResolver") );
#endif

////// radio
    Phonon::AudioOutput* audioOutput = new Phonon::AudioOutput( Phonon::MusicCategory, this );
	audioOutput->setVolume( AppSettings().volume() );

    QString audioOutputDeviceName = moose::Settings().audioOutputDeviceName();
    if (audioOutputDeviceName.size())
    {
        foreach (Phonon::AudioOutputDevice d, Phonon::BackendCapabilities::availableAudioOutputDevices())
            if (d.name() == audioOutputDeviceName) {
                audioOutput->setOutputDevice( d );
                break;
            }
    }

	m_radio = new Radio( audioOutput );
	connect( m_radio, SIGNAL(tuningIn( RadioStation )), m_stateMachine, SLOT(onRadioTuningIn( RadioStation )) );
    connect( m_radio, SIGNAL(trackSpooled( Track )), m_stateMachine, SLOT(onRadioTrackSpooled( Track )) );
    connect( m_radio, SIGNAL(trackStarted( Track )), m_stateMachine, SLOT(onRadioTrackStarted( Track )) );
    connect( m_radio, SIGNAL(stopped()), m_stateMachine, SLOT(onRadioStopped()) );
    connect( m_radio, SIGNAL(error( int, QVariant )), SLOT(onRadioError( int, QVariant )) );
    
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
    
    connect( qApp, SIGNAL(userGotInfo( WsReply* )), SLOT(onUserGotInfo( WsReply* )) );
}


App::~App()
{
    AppSettings().setVolume( m_radio->audioOutput()->volume() );
    
    delete m_scrobbler;
    delete m_radio;
    delete m_resolver;
}


void
App::setMainWindow( MainWindow* window )
{
    m_mainWindow = window;
    
    window->ui.scrobble->setChecked( false ); // if true we set it up the bomb later in this function

    connect( window->ui.love, SIGNAL(triggered( bool )), SLOT(love( bool )) );
    connect( window->ui.ban,  SIGNAL(triggered()), SLOT(ban()) );
	connect( window->ui.skip, SIGNAL(triggered()), m_radio, SLOT(skip()) );
    connect( window->ui.scrobble, SIGNAL(toggled( bool )), SLOT(setScrobblingEnabled( bool )) );

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

    if (AppSettings().isScrobblingEnabled())
    {
        // only do if true, otherwise the state machine will be fucked
        setScrobblingEnabled( true );
    }

    connect( m_scrobbler, SIGNAL(status( int )), window->ui.diagnostics, SLOT(scrobbleActivity( int )) );
}


void
App::setScrobblingEnabled( bool b )
{
    if (sender() != m_mainWindow->ui.scrobble)
    {
        Q_ASSERT( m_mainWindow->ui.scrobble->isChecked() != b );
        m_mainWindow->ui.scrobble->setChecked( b ); //recursive
    }
    else if (b)
    {
        connect( m_stateMachine, SIGNAL(trackSpooled( Track )), m_scrobbler, SLOT(nowPlaying( Track )) );
        connect( m_stateMachine, SIGNAL(scrobblePointReached( Track )), m_scrobbler, SLOT(cache( Track )) );
        
        connect( new WsConnectionMonitor( m_scrobbler ), SIGNAL(up()), m_scrobbler, SLOT(rehandshake()) );
        
        Track t = m_stateMachine->track();
        if (!t.isNull())
            m_scrobbler->nowPlaying( t );
    }
    else {
        disconnect( m_stateMachine, 0, m_scrobbler, 0 );
        delete m_scrobbler->findChild<WsConnectionMonitor*>();
    }
    
    AppSettings().setScrobblingEnabled( b );
}


void
App::onWsError( Ws::Error e )
{
    switch (e)
    {
        case Ws::OperationFailed:
            //TODOCOPY
            //TODO use the non intrusive status messages
            QMessageBoxBuilder( m_mainWindow )
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
            QMessageBoxBuilder( m_mainWindow )
                .setIcon( QMessageBox::Warning )
                .setTitle( tr("Upgrade Required") )
                .setText( tr("Scrobbling will not work because this software is too old.") )
                .exec();
            break;

        case Scrobbler::ErrorInvalidSessionKey:
            logout();
            break;

        case Scrobbler::ErrorBadTime:
            QMessageBoxBuilder( m_mainWindow )
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
            BootstrapDialog( m_listener, m_mainWindow ).exec();
        }
    #endif
    }
    catch (std::runtime_error& e)
    {
        qWarning() << e.what();
    }
} 


void
App::onTrackSpooled( const Track& t )
{
    if (t.isNull()) return;
    
    qDebug() << t;

    if (t.source() == Track::Player && t.isMp3() && moose::Settings().fingerprintingEnabled())
    {
        FingerprintId fpid = Fingerprint( t ).id();
        
        if (t.mbid().isNull() || fpid.isNull())
        {
            ExtractIdentifiersJob* job = new ExtractIdentifiersJob( t );
            QThreadPool::globalInstance()->start( job );
            
            connect( job, SIGNAL(fingerprinted( Track )), m_mainWindow->ui.diagnostics, SLOT(fingerprinted( Track )) );
        }
    }
}


void
App::onTrackUnspooled( const Track& t )
{
    Scrobble s( t );
    bool const enabled = m_mainWindow->ui.scrobble->isChecked();
    
    // always submit loved tracks, scrobble point reached or not
    // always submit banned tracks, scrobble point reached or not
    // only submit skipped tracks, scrobble point reached or not, but only if scrobbling is on

    if (s.isLoved() || s.isBanned() || s.isSkipped() && enabled && s.source() == Track::LastFmRadio)
    {
        m_scrobbler->cache( t );
        m_scrobbler->submit();
    }
    else if (enabled)
        m_scrobbler->submit();
}


void
App::love( bool b )
{
	MutableTrack t = m_stateMachine->track();

	if (b) {
		t.love();
        m_scrobbler->cache( t );
    }
	else
		t.unlove();
}


void
App::ban()
{
	MutableTrack t = m_stateMachine->track();
	t.ban();
	m_scrobbler->cache( t );
	m_radio->skip();
}


void
App::open( const QUrl& url )
{
    open( RadioStation( url.toString() ) );
}


void
App::open( const RadioStation& station )
{
    if (m_mainWindow->ui.localRadio->isChecked())
    {
        openLocalContent( station );
        return;
    }    
    
    AbstractTrackSource* source = station.isLegacyPlaylist()
            ? (AbstractTrackSource*) new LegacyTuner( station, CoreSettings().value( "Password" ).toString() )
            : (AbstractTrackSource*) new Tuner( station );

    m_radio->play( station, source );
}


void
App::openXspf( const QUrl& url )
{
    XspfResolvingTrackSource* src = new XspfResolvingTrackSource( m_resolver, url );
    m_radio->play( RadioStation( "XSPF" ), src );
    src->start();
}


#include "LocalRadioTrackSource.h"
void
App::openLocalContent( const RadioStation& station )
{
    //FIXME this synconicity is evil, but so is asyncronicity here
    
    LocalRqlResult* result = localRql()->startParse( station.rql() );
    
    if (!result)
    {
        QMessageBoxBuilder( m_mainWindow )
                .setTitle( tr("Local Radio Error") )
                .setText( tr("Could not load plugin") )
                .sheet()
                .exec();
        return;
    }
    
    QEventLoop loop;
    connect( result, SIGNAL(parseGood( unsigned )), &loop, SLOT(quit()) );
    connect( result, SIGNAL(parseBad( int, QString, int )), &loop, SLOT(quit()) );
    loop.exec();

    LocalRadioTrackSource* source = new LocalRadioTrackSource( result );
    m_radio->play( RadioStation( "Local Content" ), source  );
    source->start();
}


void
App::open( AbstractTrackSource* source )
{
    m_radio->play( RadioStation( "Local Content" ), source  );
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
                submitTwiddleCache( args.value( 2 ) );
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


void
App::submitTwiddleCache( const QString& path )
{    
    IPodScrobbleCache cache( path );
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
}


LocalRql*
App::localRql()
{
    return m_localRql;
}


#include "the/app.h"
namespace The
{
    Radio& radio() { return *app().m_radio; }
    MainWindow& mainWindow() { return *app().m_mainWindow; }
}

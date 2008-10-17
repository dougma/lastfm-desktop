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
#include "MbidJob.h"
#include "Settings.h"
#include "ipod/BatchScrobbleDialog.h"
#include "ipod/IPodScrobbleCache.h"
#include "mac/MacStyle.h"
#include "mac/ITunesListener.h"
#include "mac/ITunesPluginInstaller.h"
#include "player/PlayerListener.h"
#include "player/PlayerMediator.h"
#include "radio/RadioWidget.h"
#include "widgets/DiagnosticsDialog.h"
#include "widgets/MainWindow.h"
#include "app/twiddly/IPodScrobble.h"
#include "lib/lastfm/core/QMessageBoxBuilder.h"
#include "lib/lastfm/scrobble/Scrobbler.h"
#include "lib/lastfm/radio/Radio.h"
#include "lib/lastfm/fingerprint/FingerprintIdRequest.h"
#include "lib/unicorn/BackgroundJobQueue.h"
#include <QLineEdit>
#include <QSystemTrayIcon>
#include <phonon/audiooutput.h>

#ifdef WIN32
    #include "legacy/disableHelperApp.cpp"
#endif


App::App( int& argc, char** argv ) 
   : Unicorn::Application( argc, argv )
{
#ifdef Q_WS_MAC
    // I have to set it on the whole application as QMainWindow grabs some
    // pixelmetrics when it is created, which means a post setStyle doesn't
    // work
    setStyle( new UnicornMacStyle );
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
    s.setValue( "Path", applicationFilePath() );

    m_q = new BackgroundJobQueue;
    
    PlayerListener* listener = new PlayerListener( this );
    connect( listener, SIGNAL(bootstrapCompleted( QString )), SLOT(onBootstrapCompleted( QString )) );
    
    m_playerMediator = new PlayerMediator( listener );
    connect( m_playerMediator, SIGNAL(playerChanged( QString )), SIGNAL(playerChanged( QString )) );
    connect( m_playerMediator, SIGNAL(stateChanged( State, Track )), SIGNAL(stateChanged( State, Track )) );
    connect( m_playerMediator, SIGNAL(stopped()), SIGNAL(stopped()) );
    connect( m_playerMediator, SIGNAL(trackSpooled( Track, StopWatch* )), SIGNAL(trackSpooled( Track, StopWatch* )) );
    connect( m_playerMediator, SIGNAL(trackUnspooled( Track )), SIGNAL(trackUnspooled( Track )) );
    connect( m_playerMediator, SIGNAL(scrobblePointReached( Track )), SIGNAL(scrobblePointReached( Track )) ); 

    connect( m_playerMediator, SIGNAL(trackSpooled( Track )), SLOT(onTrackSpooled( Track )) );
    
#ifdef Q_WS_MAC
    new ITunesListener( listener->port(), this );
#endif
    
    m_scrobbler = new Scrobbler( "ass" );
    connect( m_playerMediator, SIGNAL(trackSpooled( Track )), m_scrobbler, SLOT(nowPlaying( Track )) );
    connect( m_playerMediator, SIGNAL(trackUnspooled( Track )), m_scrobbler, SLOT(submit()) );
    connect( m_playerMediator, SIGNAL(scrobblePointReached( Track )), m_scrobbler, SLOT(cache( Track )) );

	m_radio = new Radio( new Phonon::AudioOutput );
	m_radio->audioOutput()->setVolume( 0.8 ); //TODO rememeber

	connect( m_radio, SIGNAL(tuningIn( RadioStation )), m_playerMediator, SLOT(onRadioTuningIn( RadioStation )) );
    connect( m_radio, SIGNAL(trackSpooled( Track )), m_playerMediator, SLOT(onRadioTrackSpooled( Track )) );
    connect( m_radio, SIGNAL(trackStarted( Track )), m_playerMediator, SLOT(onRadioTrackStarted( Track )) );
    connect( m_radio, SIGNAL(stopped()), m_playerMediator, SLOT(onRadioStopped()) );
    
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
    
    parseArguments( arguments() );
}


App::~App()
{
    delete m_scrobbler;
    delete m_q;
    delete m_radio;
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
App::onTrackSpooled( const Track& t )
{
    if( t.isNull() ) return;
    
#ifdef Q_OS_MAC
    if( t.source() == Track::Player )
    {
        FingerprintIdRequest * fpReq = new FingerprintIdRequest( t, this );
        connect( fpReq, SIGNAL( unknownFingerprint( QString )), SLOT( onUnknownFingerprint( QString )));
        fpReq->setAutoDelete( false );
        fpReq->start();
    }
#endif
    
    if ( t.mbid().isNull())
    {
        m_q->enqueue( new MbidJob( t ) );
    }
}


#ifdef Q_OS_MAC
void 
App::onUnknownFingerprint( QString )
{
    FingerprintIdRequest* fpReq = static_cast< FingerprintIdRequest* >( sender());
    disconnect( fpReq, SIGNAL( unknownFingerprint( QString )), this, SLOT( onUnknownFingerprint( QString )));
    fpReq->setAutoDelete( true );
    fpReq->start( Fp::FullMode );
}
#endif


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
    m_radio->play( RadioStation( url.toString() ) );
}


namespace //anonymous namespace, keep to this file
{
    enum Argument
    {
        LastFmUrl,
        Pause, //toggles pause
        Skip,
		SubmitIPodScrobbles,
        Message,
        Unknown
    };
    
    Argument argument( const QString& arg )
    {
        if (arg == "--pause") return Pause;
        if (arg == "--skip") return Skip;
		if (arg == "--twiddled") return SubmitIPodScrobbles;
        
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
			case Message:
				break;
			
			case SubmitIPodScrobbles:
            {
                IPodScrobbleCache cache( args.value( 2 ) );
                QList<Track> tracks = cache.tracks();
                if (cache.insane() || Settings().alwaysConfirmIPodScrobbles())
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
                    for (int y = n; y; --y)
                        i.insert( s );
                    s.setPlayCount( 1 );
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

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
#include "PlaybackEvent.h"
#include "PlayerListener.h"
#include "PlayerManager.h"
#include "radio/RadioWidget.h"
#include "Settings.h"
#include "version.h"
#include "mac/ITunesListener.h"
#include "scrobbler/Scrobbler.h"
#include "widgets/DiagnosticsDialog.h"
#include "widgets/MainWindow.h"
#include "lib/unicorn/MessageBoxBuilder.h"
#include <QLineEdit>
#include <QSystemTrayIcon>

#ifdef WIN32
    #include "legacy/disableHelperApp.cpp"
#endif


App::App( int argc, char** argv ) 
   : Unicorn::Application( argc, argv )
{
    // IMPORTANT don't allow any GUI thread message loops to run during this
    // ctor! Things will crash in interesting ways!
    //TODO bootstrapping
    
    Settings::instance = new Settings( VERSION, applicationFilePath() );

    m_playerListener = new PlayerListener( this );
    connect( m_playerListener, SIGNAL(bootstrapCompleted( QString, QString )), SLOT(onBootstrapCompleted( QString, QString )) );
    
    m_playerManager = new PlayerManager( m_playerListener );
    connect( m_playerManager, SIGNAL(event( int, QVariant )), SLOT(onAppEvent( int, QVariant )) );
    
#ifdef Q_WS_MAC
    new ITunesListener( m_playerListener->port(), this );
#endif
    
    m_scrobbler = new Scrobbler( The::settings().username(), The::settings().sessionKey() );
    m_radio = new RadioWidget;

    DiagnosticsDialog::observe( m_scrobbler );

    setQuitOnLastWindowClosed( false );

    //TODO do once?
#ifdef WIN32
    Legacy::disableHelperApp();
#endif
}


App::~App()
{
    delete m_radio;
    delete m_scrobbler;
    delete Settings::instance;
}


void
App::setMainWindow( MainWindow* window )
{
    m_mainWindow = window;

    connect( window->ui.love, SIGNAL(triggered()), SLOT(love()) );
    connect( window->ui.ban,  SIGNAL(triggered()), SLOT(ban()) );
    connect( window->ui.tunein, SIGNAL(triggered()), m_radio, SLOT(show()) );
    connect( window->ui.logout, SIGNAL(triggered()), SLOT(logout()) );
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
}


PlaybackState::Enum
App::state() const
{
    return m_playerManager->state();
}


void
App::onAppEvent( int e, const QVariant& d )
{
    switch (e)
    {
        case PlaybackEvent::TrackChanged:
            m_scrobbler->submit();
            // FALL THROUGH
        case PlaybackEvent::PlaybackStarted:
        {
            Track t = d.value<ObservedTrack>();
            m_scrobbler->nowPlaying( t );

            // no tooltips on mac
        #ifndef Q_WS_MAC
            m_trayIcon->setToolTip( t.toString() );
        #endif
            break;
        }            

        case PlaybackEvent::ScrobblePointReached:
            m_scrobbler->cache( d.value<ObservedTrack>() );
            break;

        case PlaybackEvent::PlaybackEnded:
            m_scrobbler->submit();
            break;
    }

    emit event( e, d );
}


void
App::onWsError( Ws::Error e )
{
    switch (e)
    {
        case Ws::InvalidSessionKey:
            logout();
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
App::onBootstrapCompleted( const QString& playerId, const QString& username )
{}


void
App::love()
{}


void
App::ban()
{}


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


namespace The
{
    App& app() { return *(App*)qApp; }
}

/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd                                       *
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
#include "RadioPlayer.h"
#include "Settings.h"
#include "version.h"
#include "mac/ITunesListener.h"
#include "scrobbler/Scrobbler.h"
#include "widgets/DiagnosticsDialog.h"
#include "widgets/LoginDialog.h"
#include "widgets/MainWindow.h"
#include "lib/unicorn/LastMessageBox.h"


App::App( int argc, char** argv ) 
   : QApplication( argc, argv )
{
#ifdef Q_WS_MAC
    if (QSysInfo::MacintoshVersion < QSysInfo::MV_10_4)
    {
        LastMessageBox::critical( 
                QObject::tr( "Unsupported OS X Version" ),
                QObject::tr( "We are sorry, but Last.fm requires OS X Tiger or above." ) );
        throw 1; //FIXME using exceptions for flow control? eww!
    }
#endif

    Settings::instance = new Settings( VERSION, applicationFilePath() );

    if (The::settings().username().isEmpty() || The::settings().logOutOnExit())
    {
        LoginDialog d;
        if (d.exec() == QDialog::Accepted)
        {
            // we shouldn't store this really, if LogOutOnExit is enabled
            // but we delete the setting on exit, and it means other apps can
            // log in while the client is loaded, and prevents us having to 
            // store these datas for the use case where LogOutOnExit is disabled
            // during the session
            Unicorn::QSettings().setValue( "Username", d.username() );
            Unicorn::QSettings().setValue( "Password", d.password() );

            //TODO bootstrapping
        }
        else
        {
            throw 0; //FIXME using exceptions for flow control? eww!
        }
    }

    m_playerListener = new PlayerListener( this );
    connect( m_playerListener, SIGNAL(bootstrapCompleted( QString, QString )), SLOT(onBootstrapCompleted( QString, QString )) );
    
    m_playerManager = new PlayerManager( m_playerListener );
    connect( m_playerManager, SIGNAL(event( int, QVariant )), SLOT(onAppEvent( int, QVariant )) );
    
#ifdef Q_WS_MAC
    new ITunesListener( m_playerListener->port(), this );
#endif
    
    m_scrobbler = new Scrobbler( The::settings().username(), The::settings().password() );
    m_radio = new RadioPlayer( The::settings().username(), The::settings().password() );

    m_radio->play( "lastfm://user/mxcl/" );

    DiagnosticsDialog::observe( m_scrobbler );
}


App::~App()
{
    // we do this here, rather than in the SettingsDialog in case the user
    // changes their mind
    if (The::settings().logOutOnExit())
    {
        Unicorn::QSettings().remove( "Username" );
        Unicorn::QSettings().remove( "Password" );
    }

    delete Settings::instance;
}


void
App::setMainWindow( MainWindow* window )
{
    connect( window->ui.love, SIGNAL(triggered()), SLOT(love()) );
    connect( window->ui.ban,  SIGNAL(triggered()), SLOT(ban()) );
    connect( window->ui.skip, SIGNAL(triggered()), m_radio, SLOT(skip()) );
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
            m_scrobbler->nowPlaying( The::playerManager().track() );
            break;

        case PlaybackEvent::ScrobblePointReached:
            m_scrobbler->cache( The::playerManager().track() );
            break;

        case PlaybackEvent::PlaybackEnded:
            m_scrobbler->submit();
            break;
    }

    emit event( e, d );
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


namespace The
{
    App& app() { return *(App*)qApp; }
    PlayerManager& playerManager() { return The::app().playerManager(); }
}

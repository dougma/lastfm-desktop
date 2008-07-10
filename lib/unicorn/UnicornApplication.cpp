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

#include "UnicornApplication.h"
#include "LastMessageBox.h"
#include "UnicornSettings.h"
#include "widgets/LoginDialog.h"


Unicorn::Application::Application( int argc, char** argv ) throw( StubbornUserException, UnsupportedPlatformException )
                    : QApplication( argc, argv ),
                      m_logoutAtQuit( false )
{
#ifdef Q_WS_MAC
    if (QSysInfo::MacintoshVersion < QSysInfo::MV_10_4)
    {
        LastMessageBox::critical( 
                QObject::tr( "Unsupported OS X Version" ),
                QObject::tr( "We are sorry, but Last.fm requires OS X Tiger or above." ) );
        throw UnsupportedPlatformException();
    }
#endif

    Settings s;

    if (s.username().isEmpty() || s.sessionKey().isEmpty() || s.logOutOnExit())
    {
        LoginDialog d;
        if (d.exec() == QDialog::Accepted)
        {
            // if LogOutOnExit is enabled, really, we shouldn't store these,
            // but it means other Unicorn apps can log in while the client is 
            // loaded, and we delete the settings on exit if logOut is on
            Unicorn::QSettings().setValue( "Username", d.username() );
            Unicorn::UserQSettings s;
            s.setValue( "Username", d.username() );
            s.setValue( "SessionKey", d.sessionKey() );
        }
        else
        {
            throw StubbornUserException();
        }
    }
}


Unicorn::Application::~Application()
{
    // we do this here, rather than when the setting is changed because if we 
    // did it then the user would be unable to change their mind
    if (Unicorn::Settings().logOutOnExit() || m_logoutAtQuit)
    {
        Unicorn::UserQSettings s;
        s.remove( "Password" );
        s.remove( "SessionKey" );
        Unicorn::QSettings().remove( "Username" ); // do after the UserQSettings or it doesn't work!
    }
}

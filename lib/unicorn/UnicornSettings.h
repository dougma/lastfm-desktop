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

#ifndef UNICORN_SETTINGS_H
#define UNICORN_SETTINGS_H

#include "lib/DllExportMacro.h"
#include <QSettings>
#include <QString>
#include "app/client/version.h" //FIXME for PRODUCT_NAME


namespace Unicorn
{
    /** Use this if you need to store or access settings stored in the 
      * AudioScrobbler namespace
      * TODO should use Software/Last.fm on Windows
      *      fm.last.plist on mac
      *      .config/Last.fm on Linux
      */
    class QSettings : public ::QSettings
    {
    public:
        QSettings() : ::QSettings( "Last.fm", PRODUCT_NAME )
        {}
    };
    
    
    /** Clearly no use until a username() has been assigned. But this is 
      * automatic if you use Unicorn::Application anyway. */
    class UserQSettings : public Unicorn::QSettings
    {
    public:
        UserQSettings();
    };
    
    
    /** Settings that may be of use to the entire Last.fm suite 
      */
    class UNICORN_DLLEXPORT Settings
    {
    public:
        Settings()
        {}

        QString username() const { return QSettings().value( "Username" ).toString(); }
        QString password() const { return UserQSettings().value( "Password" ).toString(); }
        QString sessionKey() const { return UserQSettings().value( "SessionKey", "" ).toString(); }

        bool isUseProxy() const { return QSettings().value( "ProxyEnabled" ).toInt() == 1; }
        QString proxyHost() const { return QSettings().value( "ProxyHost" ).toString(); }
        int proxyPort() const { return QSettings().value( "ProxyPort" ).toInt(); }
        QString proxyUser() const { return QSettings().value( "ProxyUser" ).toString(); }
        QString proxyPassword() const { return QSettings().value( "ProxyPassword" ).toString(); }

        // all Unicorn::Applications obey this
        bool logOutOnExit() const { return UserQSettings().value( "LogOutOnExit", false ).toBool(); }

        /** @returns one of our pre-defined 2-letter language codes */
        QString language() const;
    };
    

    inline UserQSettings::UserQSettings()
    {
        QString const username = Unicorn::Settings().username();
        beginGroup( username );
        // it shouldn't be possible, since Unicorn::Application enforces 
        // assignment of the username parameter before anything else
        Q_ASSERT( !username.isEmpty() );
    }


    class UNICORN_DLLEXPORT MutableSettings : private Settings
    {
        void setPassword(); // undefined, as basically, you're not allowed
        void setUsername(); // to do this, Unicorn::Application can though

    public:
        MutableSettings()
        {}

        void setUseProxy( bool v ) { QSettings().setValue( "ProxyEnabled", v ? "1" : "0" ); }
        void setProxyHost( QString v ) { QSettings().setValue( "ProxyHost", v ); }
        void setProxyPort( int v ) { QSettings().setValue( "ProxyPort", v ); }
        void setProxyUser( QString v ) { QSettings().setValue( "ProxyUser", v ); }
        void setProxyPassword( QString v ) { QSettings().setValue( "ProxyPassword", v ); }
        void setLanguage( QString langCode ) { QSettings().setValue( "AppLanguage", langCode ); }
        void setLogOutOnExit( bool b ) { UserQSettings().setValue( "LogOutOnExit", b ); }
    };
}



#endif

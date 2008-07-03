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
        QSettings() : ::QSettings( "Last.fm", "AudioScrobbler" )
        {}
    };
    
    
    /** Clearly no use until a username() has been assigned. But this is 
      * automatic if you use Unicorn::Application anyway. */
    class UserSettings : public Unicorn::QSettings
    {
    public:
        UserSettings();
    };
    
    
    /** Settings that may be of use to the entire Last.fm suite 
      */
    class UNICORN_DLLEXPORT Settings
    {
    public:
        Settings()
        {}

        QString username() const { return QSettings().value( "Username" ).toString(); }
        // perhaps should be a UserSetting, but well, only one person can be 
        // logged in at a time and we forget passwords for security reasons
        // inbetween user switching. So really this makes sense.
        QString password() const { return QSettings().value( "Password" ).toString(); }

        bool isUseProxy() const { return QSettings().value( "ProxyEnabled" ).toInt() == 1; }
        QString proxyHost() const { return QSettings().value( "ProxyHost" ).toString(); }
        int proxyPort() const { return QSettings().value( "ProxyPort" ).toInt(); }
        QString proxyUser() const { return QSettings().value( "ProxyUser" ).toString(); }
        QString proxyPassword() const { return QSettings().value( "ProxyPassword" ).toString(); }

        // all Unicorn::Applications obey this
        bool logOutOnExit() const { return UserSettings().value( "LogOutOnExit", false ).toBool(); }

        /** @returns one of our pre-defined 2-letter language codes */
        QString language() const;
    };
    

    inline UserSettings::UserSettings()
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
        void setLogOutOnExit( bool b ) { UserSettings().setValue( "LogOutOnExit", b ); }
    };
}

#endif

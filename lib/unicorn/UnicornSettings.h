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
#include "lib/lastfm/core/CoreSettings.h" //CORE_ORGANISATION_DOMAIN
#include <QSettings>
#include <QString>


namespace Unicorn
{   
    /** Clearly no use until a username() has been assigned. But this is 
      * automatic if you use Unicorn::Application anyway. */
    class UserSettings : public CoreSettings
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

        QString username() const { return CoreSettings().value( "Username" ).toString(); }
        QString sessionKey() const { return UserSettings().value( "SessionKey", "" ).toString(); }

        // all Unicorn::Applications obey this
        bool logOutOnExit() const { return UserSettings().value( "LogOutOnExit", false ).toBool(); }
    };
    

    inline UserSettings::UserSettings()
    {
        QString const username = Settings().username();
        beginGroup( username );
        // it shouldn't be possible, since Unicorn::Application enforces 
        // assignment of the username parameter before anything else
        Q_ASSERT( !username.isEmpty() );
    }


    class UNICORN_DLLEXPORT MutableSettings : private Settings
    {
        void setSessionKey(); // undefined, as basically, you're not allowed
        void setUsername(); // to do this, Unicorn::Application can though

    public:
        MutableSettings()
        {}

        void setLanguage( QString langCode ) { CoreSettings().setValue( "AppLanguage", langCode ); }
        void setLogOutOnExit( bool b ) { UserSettings().setValue( "LogOutOnExit", b ); }
    };
}

#endif

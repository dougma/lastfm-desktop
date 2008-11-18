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

#ifndef SETTINGS_H
#define SETTINGS_H

#include "lib/lastfm/core/CoreSettings.h"
#include "lib/lastfm/scrobble/ScrobblePoint.h"
#include "lib/unicorn/UnicornSettings.h"


namespace moose
{
    /** Clearly no use until a username() has been assigned. But this is 
      * automatic if you use Unicorn::Application anyway. 
      */
    class UserSettings : public QSettings
    {
    public:
        UserSettings()
        {
            QString const username = CoreSettings().value( "Username" ).toString();
            beginGroup( username );
            // it shouldn't be possible, since Unicorn::Application enforces 
            // assignment of the username parameter before anything else
            Q_ASSERT( !username.isEmpty() );
        }
    };


    /** Usage: only put interesting settings in here, which mostly means ones set in
      * in the settings dialog. For class local settings, just make a small 
      * QSettings local derived class.
      */
    struct Settings
    {
        int scrobblePoint() const { return UserSettings().value( "ScrobblePoint", ScrobblePoint::kDefaultScrobblePoint ).toInt(); }
        bool alwaysConfirmIPodScrobbles() const { return UserSettings().value( "AlwaysConfirmIPodScrobbles", true ).toBool(); }
        bool fingerprintingEnabled() const { return UserSettings().value( "FingerprintingEnabled", true ).toBool(); }
        bool iPodScrobblingEnabled() const { return UserSettings().value( "iPodScrobblingEnabled", true ).toBool(); }
    };

    struct MutableSettings
    {
        void setLogOutOnExit( bool b ) { Unicorn::UserSettings().setValue( "LogOutOnExit", b ); }
        void setScrobblePoint( int scrobblePoint ) { UserSettings().setValue( "ScrobblePoint", scrobblePoint ); }
        void setIPodScrobblingEnabled( bool b ) { UserSettings().setValue( "iPodScrobblingEnabled", b ); }
        void setAlwaysConfirmIPodScrobbles( bool b ) { UserSettings().setValue( "AlwaysConfirmIPodScrobbles", b ); }
        void setFingerprintingEnabled( bool b ) { UserSettings().setValue( "FingerprintingEnabled", b ); }
    };

#ifdef WIN32
    struct HklmSettings : QSettings
    {
        HklmSettings() : QSettings( "HKEY_LOCAL_MACHINE\\Software\\Last.fm\\Client", QSettings::NativeFormat )
        {}
    };
#endif
}


#ifdef WIN32
struct Plugin
{
    QString name;
    QString version;
    
    struct Settings : HklmSettings
    {
        Settings()
        {
            beginGroup( "Plugins" );
        }
    };
    
    static QList<Plugin> installed()
    {
        QList<Plugin> plugins;
        
        foreach (QString group, s.childGroups())
        {            
            Settings s;
            s.beginGroup( group );

            Plugin p;            
            p.name = s.value( "Name" ).toString();
            
            //If the plugin has been added but not installed name.size() == 0
            if (p.name.isEmpty())
                continue;

            p.version = s.value( "Version" ).toString();
            
            plugins += p;
        }
        
        return plugins;
    };
};
#endif //WIN32

#endif

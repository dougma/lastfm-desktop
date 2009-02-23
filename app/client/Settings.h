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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#ifndef MOOSE_SETTINGS_H
#define MOOSE_SETTINGS_H

#include "lib/lastfm/scrobble/ScrobblePoint.h"
#include "lib/unicorn/UnicornSettings.h"


namespace moose
{
    using unicorn::UserSettings;
    
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
        QString audioOutputDeviceName() const { return QSettings().value( "OutputDevice" ).toString(); }
    };

    struct MutableSettings : Settings
    {
        void setScrobblePoint( int i ) { UserSettings().setValue( "ScrobblePoint", qBound( 0, i, 100 ) ); }
        void setIPodScrobblingEnabled( bool b ) { UserSettings().setValue( "iPodScrobblingEnabled", b ); }
        void setAlwaysConfirmIPodScrobbles( bool b ) { UserSettings().setValue( "AlwaysConfirmIPodScrobbles", b ); }
        void setFingerprintingEnabled( bool b ) { UserSettings().setValue( "FingerprintingEnabled", b ); }
        void setAudioOutputDeviceName( QString s ) { QSettings().setValue( "OutputDevice", s ); }
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

#include <QStringList>

struct Plugin
{
    QString id;
    QString name;
    QString version;
    
    struct Settings : moose::HklmSettings
    {
        Settings()
        {
            beginGroup( "Plugins" );
        }
    };
    
    static QList<Plugin> installed()
    {
        QList<Plugin> plugins;
        Settings s;

        foreach (QString group, s.childGroups())
        {            
            s.beginGroup( group );

            Plugin p;            
            p.name = s.value( "Name" ).toString();
            
            //If the plugin has been added but not installed name.size() == 0
            if (p.name.isEmpty())
                continue;

            p.version = s.value( "Version" ).toString();
            p.id = group;
            
            plugins += p;

            s.endGroup();
        }
        
        return plugins;
    };
};
#endif //WIN32

#endif

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

#ifndef MOOSE_SETTINGS_H
#define MOOSE_SETTINGS_H

#ifdef QT_CORE_LIB

#include "lib/unicorn/UnicornSettings.h"
#include "lib/lastfm/scrobble/ScrobblePoint.h"


/** Usage: only put interesting settings in here, which mostly means ones set in
  * in the settings dialog. For class local settings, just make a small 
  * QSettings local derived class */
class Settings : public Unicorn::Settings
{
    /** we force this object to be a singleton so that we force passing these
      * two parameters as soon as possible, they are frankly, important */
    Settings( const QString& version, const QString& path );

    friend class App;

    bool m_weWereJustUpgraded;
    
public:
    int scrobblePoint() const { return QSettings().value( "ScrobblePoint", ScrobblePoint::kDefaultScrobblePoint ).toInt(); }

    /** The UniqueApplication id */
    static const char* id();
};


class MutableSettings : public Unicorn::MutableSettings
{
public:
    MutableSettings( const ::Settings& )
    {}

    void setScrobblePoint( int scrobblePoint ) { QSettings().setValue( "ScrobblePoint", scrobblePoint ); }
};


#else
    /** This section should contain inline functions only, and have no dependencies
      * on anything that requires external linkage, it is used by the iTunes
      * plugin and Twiddly, and thus has to be pure c++
      * @author <max@last.fm> 
      */
    #include "version.h"

    struct Settings
    {
        /** The UniqueApplication id */
        static const char* id();

    #ifdef( WIN32 )
        std::wstring path();
    #elif defined( __APPLE__ )
        std::string path();
    #endif
    };
#endif


inline const char* Settings::id() { return "Lastfm-F396D8C8-9595-4f48-A319-48DCB827AD8F"; }

#endif

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

#ifndef MOOSE_CONFIG_H
#define MOOSE_CONFIG_H

#ifdef QT_CORE_LIB
    #include "lib/unicorn/UnicornSettings.h"
    #include "lib/lastfm/scrobble/ScrobblePoint.h"

    /** Usage: only put interesting settings in here, which mostly means ones set in
      * in the settings dialog. For class local settings, just make a small 
      * QSettings local derived class.
      *
      * Named Config as has elements of the configuration in too, and is 
      * accessed from other projects, so would be confusing to refer to this
      * as the Settings, when it is in fact The Configuration of the Moose
      * subproject.
      */
    struct MooseConfig : public Unicorn::Settings
    {
        int scrobblePoint() const { return QSettings().value( "ScrobblePoint", ScrobblePoint::kDefaultScrobblePoint ).toInt(); }
        bool alwaysConfirmIPodScrobbles() const { return true; }
        
        /** The UniqueApplication id */
        static const char* id();
    };


    struct MutableMooseConfig : public Unicorn::MutableSettings
    {
        void setScrobblePoint( int scrobblePoint ) { QSettings().setValue( "ScrobblePoint", scrobblePoint ); }
    };


#else
    #include "common/c++/string.h"

    /** This section should contain inline functions only, and have no dependencies
      * on anything that requires external linkage, it is used by the iTunes
      * plugin and Twiddly, and thus has to be pure c++
      * @author <max@last.fm> 
      */
    struct MooseConfig
    {
        /** The UniqueApplication id */
        static const char* id();
        static COMMON_STD_STRING defaultPath();
    };


    #ifdef __APPLE__
    inline std::string MooseConfig::defaultPath() { return "/Applications/Last.fm.app/Contents/MacOS/Last.fm"; }
    #elif !defined( WIN32 )
    inline std::string MooseConfig::defaultPath() { return "/usr/bin/last.fm"; }
    #endif
#endif


inline const char* MooseConfig::id() { return "Lastfm-F396D8C8-9595-4f48-A319-48DCB827AD8F"; }

#endif

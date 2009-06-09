/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef MOOSE_H
#define MOOSE_H

#include "lib/unicorn/UnicornSettings.h"
#include <lastfm/misc.h>


namespace moose
{
    static inline const char* id() { return "Lastfm-F396D8C8-9595-4f48-A319-48DCB827AD8F"; }
    /** passed to QCoreApplication::setApplicationName() */
    static inline const char* applicationName() { return "Last.fm"; }

    static inline QString path()
    {
        QString path = unicorn::GlobalSettings( applicationName() ).value( "/Path" ).toString();
        if (path.size())
            return path;

        #ifdef __APPLE__
            return "/Applications/Last.fm.app/Contents/MacOS/Last.fm";
        #endif
        #ifdef WIN32
            path = HklmSettings().value( "Path" ).toString();
            if (path.size())
                return path;
            return lastfm::dir::programFiles().filePath( "Last.fm/Last.fm.exe" );
        #endif
    }
    
    static inline QDir dir()
    {
        return QFileInfo( path() ).absoluteDir();
    }

    enum ItemDataRole
    {
        TrackRole = Qt::UserRole,
        TimestampRole,
        CumulativeCountRole,
        SecondaryDisplayRole,
        SmallDisplayRole,
        UrlRole,
        WeightingRole,
        IdentityRole,
        TypeRole,
        HighlightRole
    };
}

#endif

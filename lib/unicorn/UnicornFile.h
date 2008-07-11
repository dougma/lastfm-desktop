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

#ifndef UNICORN_FILE_H
#define UNICORN_FILE_H

#include "UnicornDir.h"
#include "lib/DllExportMacro.h"

//TODO possibly this isn't actually unicorn material?


namespace Unicorn
{
    enum Application
    {
        Moose,
        Twiddly
    };
}


namespace UnicornFile
{
    UNICORN_DLLEXPORT inline QString log( Unicorn::Application app )
    {
        switch (app)
        {
            case Unicorn::Twiddly: return UnicornDir::logs().filePath( "Twiddly.log" );
            case Unicorn::Moose: return UnicornDir::logs().filePath( "Last.fm.log" );
        }
    }


    UNICORN_DLLEXPORT inline QString executable( Unicorn::Application app )
    {
        switch (app)
        {
            case Unicorn::Twiddly:
                #ifdef Q_WS_MAC
                    return UnicornSystemDir::bundle().filePath( "Resources/iPodScrobbler" );
                #else
                    return QDir( qApp->applicationDirPath() ).filePath( "iPodScrobbler.exe" );
                #endif

            case Unicorn::Moose:
                //TODO
                break;
        }
    }
}

#endif

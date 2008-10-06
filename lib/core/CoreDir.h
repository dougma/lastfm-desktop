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

#ifndef CORE_DIR_H
#define CORE_DIR_H

#include "lib/DllExportMacro.h"
#include <QCoreApplication>
#include <QDir>


namespace CoreDir
{
    /** @returns the path to the top-level Application Data folder
      * XP:    C:\Documents and Settings\user\Local Settings\Application Data.
      * Vista: C:\Users\user\AppData\Local
      * OSX:   ~/Library/Application Support
      * Unix:  ~/.local/share
      *
      * May return an empty string on Windows if the system call to get the
      * path fails.
      */
    CORE_DLLEXPORT QDir dataDotDot();


    /** @returns directory where application data can be stored
      * XP:    C:\Documents and Settings\user\Local Settings\Application Data\Last.fm
      * Vista: C:\Users\user\AppData\Local\Last.fm
      * OSX:   ~/Library/Application Support/Last.fm/
      * Unix:  ~/.local/share/Last.fm/
      */
    CORE_DLLEXPORT inline QDir data()
    {
        return dataDotDot().filePath( "Last.fm" );
    }
    

    /** @returns directory where logs can be stored
      * XP:    userData()
      * Vista: userData()
      * OSX:   ~/Library/Logs/Last.fm/ on OS X.
      * Unix:  userData()
      */
    CORE_DLLEXPORT inline QDir logs()
    {
        #ifdef Q_WS_MAC
            return QDir::home().filePath( "Library/Logs/Last.fm" );
        #else
            return data();    
        #endif
    }


    CORE_DLLEXPORT inline QString mainLog()
    {
    #ifdef NDEBUG
        return logs().filePath( QCoreApplication::applicationName() + ".log" );
    #else
        return logs().filePath( QCoreApplication::applicationName() + ".debug.log" );
    #endif
    }


    /** @returns path to directory for storing cached images etc. */
    CORE_DLLEXPORT inline QDir cache()
    {
        #ifdef Q_WS_MAC
            return QDir::home().filePath( "Library/Cache/Last.fm" );
        #else
            return data().filePath( "cache/" );
        #endif
    }


    CORE_DLLEXPORT inline void mkpaths()
    {
        cache().mkpath( "." );
        data().mkpath( "." );
        logs().mkpath( "." );
    }

    
#ifdef WIN32
    /** @returns the system's equivalent of c:\Program Files\ */
    CORE_DLLEXPORT QDir programFiles();
#endif
    
#ifdef Q_WS_MAC
    /** eg. /Applications/Last.fm.app/ */
    QDir bundle();
#endif
}

#endif

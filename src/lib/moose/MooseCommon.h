/***************************************************************************
 *   Copyright (C) 2005 - 2008 by                                          *
 *      Last.fm Ltd <client@last.fm>                                       *
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

#ifndef MOOSECOMMON_H
#define MOOSECOMMON_H

/** @author <erik@last.fm> */

#include "common/DllExportMacro.h"

#include "TrackInfo.h"

#include <QString>
#include <QObject>

#ifdef QT_GUI_LIB
    #include <QIcon>
#endif

#ifdef WIN32
    #define EXTENSION_PREFIX "ext_"
    #define SERVICE_PREFIX "srv_"
    #define DEBUG_SUFFIX "d"
    #define LIB_EXTENSION ".dll"

#elif defined Q_WS_X11
    #define EXTENSION_PREFIX "libext_"
    #define SERVICE_PREFIX "libsrv_"
    #define DEBUG_SUFFIX "_debug"
    #define LIB_EXTENSION ".so"

#elif defined Q_WS_MAC
    #define EXTENSION_PREFIX "libext_"
    #define SERVICE_PREFIX "libsrv_"
    #define DEBUG_SUFFIX "_debug"
    #define LIB_EXTENSION ".dylib"
#endif


namespace MooseEnums
{
    enum UserIconColour
    {
        eNone = -1,
        eRed = 0,
        eBlue,
        eGreen,
        eOrange,
        eBlack,
        eColorMax
    };
    
    enum StartNewInstanceBehaviour
    {
        StartNewInstance,
        DontStartNewInstance
    };
}


namespace MooseUtils
{
    /**
     * Returns path to named file in the app's bin/data directory.
     */
    DLLEXPORT QString
    dataPath( QString file = "" );

    /**
     * Returns path to named file in the app's user-writable save directory.
     * E.g. C:\Documents and Settings\user\Local Settings\Application Data\Last.fm\Client on Windows.
     *      ~/Library/Application Support/Last.fm/ on OS X.
     *      ~/.local/share/Last.fm/ on Unix.
     */
    DLLEXPORT QString
    savePath( QString file = "" );

    /**
     * Returns path to named file in the preferred (OS-dependant) logging directory.
     * E.g. C:\Documents and Settings\user\Local Settings\Application Data\Last.fm\Client on Windows.
     *      ~/Library/Logs/ on OS X.
     *      ~/.local/share/Last.fm/ on Unix.
     */
    DLLEXPORT QString
    logPath( QString file );

    /**
     * Returns path to directory for storing cached images etc.
     */
    DLLEXPORT QString
    cachePath();

    /**
     * Returns path to named service plugin.
     */
    DLLEXPORT QString
    servicePath( QString name );

    /**
     * Helper function to load up a named service plugin.
     */
    DLLEXPORT QObject*
    loadService( QString name );
    
    // Not exported because implementation is in header.
    // Seeing as it's a template living in a DLL, it has to be that way.
    template <class T> T*
    loadService( const QString& name )
    {
        return static_cast<T*>(loadService( name ));
    }

    /**
     * Helper function to load a named icon from disk.
     */
  #ifdef QT_GUI_LIB
    DLLEXPORT QIcon
    icon( const char *name );
  #endif

    /**
     * @returns true if the client is already running
     */
    DLLEXPORT bool
    isAlreadyRunning();

    /**
     * Sends the command to the running client instance, or starts the client 
     * then sends it, NOTE we don't currently support spaces in @p command
     */
    DLLEXPORT bool
    sendToInstance( const QString& command,
                    MooseEnums::StartNewInstanceBehaviour = MooseEnums::DontStartNewInstance );


  #ifdef WIN32
    /**
     * Stop the helper from autolaunching.
     * This is still needed on Windows to remove the autolaunch entry that was added by
     * versions prior to 1.5.
     */
    DLLEXPORT void
    disableHelperApp();
  #endif

    DLLEXPORT QStringList
    extensionPaths();

  #ifdef Q_OS_MAC
    /** eg. /Applications/Last.fm.app/ */
    QString bundleDirPath();
  #endif

} //namespace MooseUtils


namespace MooseConstants
{
    // The plugin ID used by HttpInput when submitting radio tracks to the player listener
    const QString kRadioPluginId = "radio";

    // Min size of buffer holding streamed http data, i.e the size the http
    // buffer needs to get to before we start streaming.
    const int kHttpBufferMinSize = 16 * 1024;

    // Max
    const int kHttpBufferMaxSize = 256 * 1024;

};


namespace Moose
{
    using namespace MooseUtils;
    using namespace MooseEnums;
}


#endif // MOOSECOMMON_H

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

#include "UnicornDir.h"
#include "UnicornUtils.h"
#include <QDebug>
#ifdef WIN32
#include <shlobj.h>
#endif


#ifdef Q_WS_MAC
QDir
UnicornSystemDir::bundle()
{
    return QDir( qApp->applicationDirPath() ).absoluteFilePath( "../.." );
}
#endif


QDir
UnicornDir::dataDotDot()
{
#ifdef WIN32
    if ((QSysInfo::WindowsVersion & QSysInfo::WV_DOS_based) == 0)
    {
        // Use this for non-DOS-based Windowses
        char path[MAX_PATH];
        HRESULT h = SHGetFolderPathA( NULL, 
                                      CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,
                                      NULL, 
                                      0, 
                                      path );
        if (h == S_OK)
            return QString::fromLocal8Bit( path );
    }
    return QDir::home();

#elif defined(Q_WS_MAC)
    std::string outString;

    OSErr err;

    short vRefNum = 0;
    StrFileName fileName;
    fileName[0] = 0;
    long dirId;
    err = ::FindFolder( kOnAppropriateDisk, kApplicationSupportFolderType,
        kDontCreateFolder, &vRefNum, &dirId );
    if ( err != noErr )
        return "";

    // Now we have a vRefNum and a dirID - but *not* an Unix-Path as string.
    // Lets make one based from this:

    // create a FSSpec...
    FSSpec fsspec;
    err = ::FSMakeFSSpec( vRefNum, dirId, NULL, &fsspec );
    if ( err != noErr )
        return "";

    // ...and build an FSRef based on thes FSSpec.
    FSRef fsref;
    err = ::FSpMakeFSRef( &fsspec, &fsref );
    if ( err != noErr )
        return "";

    // ...then extract the Unix Path as a C-String from the FSRef
    unsigned char path[512];
    err = ::FSRefMakePath( &fsref, path, 512 );
    if ( err != noErr )
        return "";

    // ...and copy this into the result.
    outString = (const char*)path;
    return QDir::homePath().filePath( QString::fromStdString( outString ) );

#elif defined(Q_WS_X11)
    return QDir::home().filePath( ".local/share" );

#else
    return QDir::home();
#endif
}


#ifdef WIN32
QDir
UnicornSystemDir::programFiles()
{
    char path[MAX_PATH];

    // TODO: this call is dependant on a specific version of shell32.dll.
    // Need to degrade gracefully. Need to bundle SHFolder.exe with installer
    // and execute it on install for this to work on Win98.
    HRESULT h = SHGetFolderPathA( NULL,
                                 CSIDL_PROGRAM_FILES, 
                                 NULL,
                                 0, // current path
                                 path );

    if (h != S_OK)
    {
        qCritical() << "Couldn't get Program Files dir. Possibly Win9x?";
        return "";
    }

    return QString::fromLocal8Bit( path );
}
#endif

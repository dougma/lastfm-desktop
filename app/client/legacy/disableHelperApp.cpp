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

#include "lib/unicorn/Logger.h"
#include <QFile>
#include <windows.h>
#include <shlobj.h>


namespace Legacy
{
    static QString
    helperStartupShortcutPath()
    {
        wchar_t acPath[MAX_PATH];
        HRESULT h = SHGetFolderPathW( NULL, CSIDL_STARTUP | CSIDL_FLAG_CREATE,
            NULL, 0, acPath );

        if ( h == S_OK )
        {
            QString shortcutPath;
            shortcutPath = QString::fromUtf16( reinterpret_cast<const ushort*>( acPath ) );
            if ( !shortcutPath.endsWith( "\\" ) && !shortcutPath.endsWith( "/" ) )
                shortcutPath += "\\";
            shortcutPath += "Last.fm Helper.lnk";
            return shortcutPath;
        }
        else
        {
            LOGL( 1, "Uh oh, Windows returned no STARTUP path, not going to be able to disable helper" );
            return "";
        }
    }


    void
    disableHelperApp()
    {
        // Delete the shortcut from startup and kill process
        QString shortcutPath = helperStartupShortcutPath();
        if ( !shortcutPath.isEmpty() && QFile::exists( shortcutPath ) )
        {
            bool fine = QFile::remove( shortcutPath );
            if ( !fine )
            {
                LOGL( 1, "Deletion of shortcut failed, helper will still autolaunch" );
            }
            else
            {
                LOGL( 3, "Helper shortcut removed from Startup" );
            }
        }
    }
} //namespace Legacy

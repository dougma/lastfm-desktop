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
#include <QFile>
#include <QDebug>
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
            qCritical() << "Uh oh, Windows returned no STARTUP path, not going to be able to disable helper";
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
                qCritical() << "Deletion of shortcut failed, helper will still autolaunch";
            }
            else
            {
                qDebug() << "Helper shortcut removed from Startup";
            }
        }
    }
} //namespace Legacy

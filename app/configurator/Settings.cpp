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
#include "Settings.h"


#ifdef WIN32
#ifndef QT_CORE_LIB
#include <windows.h>

std::wstring //static
MooseConfig::defaultPath()
{
    HKEY h;
    LONG lResult = RegOpenKeyExA( HKEY_CURRENT_USER, 
                                  MOOSE_HKEY_A,
                                  0,        // reserved
                                  KEY_READ, // access mask
                                  &h );

    wchar_t buffer[MAX_PATH];
    buffer[0] = L'\0';

    if ( lResult == ERROR_SUCCESS )
    {
        try
        {
            RegistryUtils::QueryString( h, L"Path", buffer, MAX_PATH, false );
        }
        catch ( const RegistryUtils::CRegistryException& )
        {
            qWarning() << "Client path not found in HKCU";
        }
        
        RegCloseKey( h );
    }

    if ( buffer[0] == L'\0' )
    {
        // Couldn't read path from HKCU, try HKLM
        lResult = RegOpenKeyExA(
                                HKEY_LOCAL_MACHINE,
                                MOOSE_HKEY_A,
                                0,              // reserved
                                KEY_READ,       // access mask
                                &h);
        
        if ( lResult == ERROR_SUCCESS )
        {
            try
            {
                RegistryUtils::QueryString( h, L"Path", buffer, MAX_PATH, false );
            }
            catch ( const RegistryUtils::CRegistryException& )
            {
                qWarning() << "Client path not found in HKLM";
            }
            
            RegCloseKey( h );
        }
    }

    if ( buffer[0] == L'\0' )
    {
        qCritical() << "Couldn't read the client path from the registry.";
        return std::wstring();
    }    

    return buffer;
}
#endif
#endif

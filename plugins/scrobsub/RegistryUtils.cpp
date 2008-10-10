/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02111-1307, USA.          *
 ***************************************************************************/

#include "RegistryUtils.h"

#include <windows.h>

#include <sstream>

using namespace std;

RegistryUtils::Registry RegistryUtils::mRegInfo;

/******************************************************************************
GetKey
******************************************************************************/
HKEY
RegistryUtils::GetKey(
                      HKEY         parentKey,
                      const WCHAR* pcSubkey,
                      bool         bCreateIfNotFound )
{
    HKEY requestedKey;
    LONG lResult;

    if ( bCreateIfNotFound )
    {
        lResult = RegCreateKeyExW(
            parentKey,
            pcSubkey,
            0,                       // reserved, must be 0
            NULL,                    // class, ignore
            REG_OPTION_NON_VOLATILE, // saves key on exit
            KEY_ALL_ACCESS,          // access mask. TODO: test this as non-admin
            NULL,                    // security attrs, NULL = default, inherit from parent
            &requestedKey,
            NULL );                   // info about whether key was created or opened
    }
    else
    {
        lResult = RegOpenKeyExW(
            parentKey,
            pcSubkey,
            0,              // reserved
            KEY_ALL_ACCESS, // access mask
            &requestedKey );
    }

    if ( lResult != ERROR_SUCCESS )
    {
        ostringstream osErr;
        osErr << "Could not retrieve registry key [" << pcSubkey <<
            "]. System error: " << lResult;
        throw CRegistryException( osErr.str() );
    }

    return requestedKey;
}


HKEY
RegistryUtils::GetKey(
                      HKEY         parentKey,
                      const char*  pcSubkey,
                      bool         bCreateIfNotFound )
{
    HKEY requestedKey;
    LONG lResult;

    if ( bCreateIfNotFound )
    {
        lResult = RegCreateKeyExA(
            parentKey,
            pcSubkey,
            0,                       // reserved, must be 0
            NULL,                    // class, ignore
            REG_OPTION_NON_VOLATILE, // saves key on exit
            KEY_ALL_ACCESS,          // access mask. TODO: test this as non-admin
            NULL,                    // security attrs, NULL = default, inherit from parent
            &requestedKey,
            NULL );                   // info about whether key was created or opened
    }
    else
    {
        lResult = RegOpenKeyExA(
            parentKey,
            pcSubkey,
            0,              // reserved
            KEY_ALL_ACCESS, // access mask
            &requestedKey );
    }

    if ( lResult != ERROR_SUCCESS )
    {
        ostringstream osErr;
        osErr << "Could not retrieve registry key [" << pcSubkey <<
            "]. System error: " << lResult;
        throw CRegistryException( osErr.str() );
    }

    return requestedKey;
}

/******************************************************************************
GetNumSubkeys
******************************************************************************/
int
RegistryUtils::GetNumSubkeys(
                        HKEY key )
{
    QueryRegInfo( key );
    return mRegInfo.mnNumSubkeys;
}

/******************************************************************************
GetSubkeyNames
******************************************************************************/

// FIXME: Why do we still need the tstring?
void
RegistryUtils::GetSubkeyNames(
                         HKEY             key,
                         vector<tstring>& vNames )
{
    int nNumSubkeys = GetNumSubkeys( key );
    DWORD nSubkeyBufferSize = ( GetLongestSubkeySize( key ) + 1 ) * sizeof( TCHAR );
    LONG lResult;
    TCHAR *sName = new TCHAR[nSubkeyBufferSize];

    for (int i = 0; i < nNumSubkeys; ++i)
    {
        // Get name of ith subkey
        DWORD nResetSize = nSubkeyBufferSize;
        lResult = RegEnumKeyEx(
            key,
            i,
            sName,
            &nResetSize,
            NULL,               // reserved, must be 0
            NULL,               // class
            NULL,               // class size
            NULL );             // last write time


        if ( lResult != ERROR_SUCCESS )
        {
            delete[] sName;
            ostringstream osErr;
            osErr << "Could not enumerate registry subkeys. System error: " <<
                lResult;
            throw CRegistryException( osErr.str() );
        }

        vNames.push_back( sName );

    }
    delete[] sName;
}

/******************************************************************************
GetLongestSubkeySize
******************************************************************************/
DWORD
RegistryUtils::GetLongestSubkeySize(
                               HKEY key)
{
    QueryRegInfo( key );
    return static_cast<DWORD>( mRegInfo.mnSizeLongestSubkey );
}

/******************************************************************************
GetLongestDataSize
******************************************************************************/
DWORD
RegistryUtils::GetLongestDataSize(
                             HKEY key )
{
    QueryRegInfo( key );
    return static_cast<DWORD>( mRegInfo.mnSizeLongestData );
}

/******************************************************************************
QueryRegInfo
******************************************************************************/
void
RegistryUtils::QueryRegInfo(
                       HKEY key )
{
    DWORD nNumSubkeys;
    DWORD nSizeLongestSubkey;
    DWORD nNumValues;
    DWORD nSizeLongestValue;
    DWORD nSizeLongestData;

    LONG lResult = RegQueryInfoKey(
        key,
        NULL,                // class, ignore
        NULL,                // class string size, ignore
        NULL,                // reserved, must be 0
        &nNumSubkeys,        // number of subkeys
        &nSizeLongestSubkey, // size in TCHARs of longest subkey,
        // includes terminating \0 on XP but not on 98. Doh!
        NULL,                // longest class string size, ignore
        &nNumValues,         // num values in this key
        &nSizeLongestValue,  // size in TCHARs of longest value string,
        // does never include terminating \0
        &nSizeLongestData,   // size in bytes of biggest data entry
        NULL,                // size of security descriptor, don't care
        NULL );              // last write time, don't care

    if ( lResult != ERROR_SUCCESS )
    {
        ostringstream osErr;
        osErr << "Could not query registry key for info. System error: " <<
            lResult;
        throw CRegistryException( osErr.str() );
    }

    mRegInfo.mKey = key;
    mRegInfo.mnNumSubkeys = nNumSubkeys;
    mRegInfo.mnSizeLongestSubkey = nSizeLongestSubkey;
    mRegInfo.mnNumValues = nNumValues;
    mRegInfo.mnSizeLongestValue = nSizeLongestValue;
    mRegInfo.mnSizeLongestData = nSizeLongestData;

}

/******************************************************************************
QueryString
******************************************************************************/
void
RegistryUtils::QueryString(
                      HKEY         key,
                      const char* pcValue,
                      char*       pcResult,
                      DWORD        nResultSize,
                      bool         bCreateIfNotFound,
                      const char* pcDefault )
{
// FIXME: this looks crazy. Why are we defining UNICODE for half the code file?
#define UNICODE
    LONG lResult = RegQueryValueExA(
        key,
        pcValue,                            // value to query
        NULL,                               // reserved
        NULL,                               // tells you what type the value is
        reinterpret_cast<LPBYTE>( pcResult ), // store result here
        &nResultSize );

    if ( lResult == ERROR_FILE_NOT_FOUND && bCreateIfNotFound )
    {
        // Create value if it wasn't found
        WriteString( key, pcValue, pcDefault );
        strncpy( pcResult, pcDefault, nResultSize );
    }
    else if ( lResult != ERROR_SUCCESS )
    {
        ostringstream osErr;
        osErr << "Could not read registry value [" << pcValue << "]. " <<
            "System error: " << lResult;
        throw CRegistryException( osErr.str() );
    }

}


/******************************************************************************
QueryString
******************************************************************************/
void
RegistryUtils::QueryString(
                           HKEY         key,
                           const WCHAR* pcValue,
                           WCHAR*       pcResult,
                           DWORD        nResultSize,
                           bool         bCreateIfNotFound,
                           const WCHAR* pcDefault )
{
    LONG lResult = RegQueryValueExW(
        key,
        pcValue,                            // value to query
        NULL,                               // reserved
        NULL,                               // tells you what type the value is
        reinterpret_cast<LPBYTE>( pcResult ), // store result here
        &nResultSize );

    if ( lResult == ERROR_FILE_NOT_FOUND && bCreateIfNotFound )
    {
        // Create value if it wasn't found
        WriteString( key, pcValue, pcDefault );
        wcsncpy( pcResult, pcDefault, nResultSize );  
    }
    else if ( lResult != ERROR_SUCCESS )
    {
        ostringstream osErr;
        osErr << "Could not read registry value [" << pcValue << "]. " <<
            "System error: " << lResult;
        throw CRegistryException( osErr.str() );
    }

}


/******************************************************************************
QueryBool
******************************************************************************/
void
RegistryUtils::QueryBool(
                    HKEY         key,
                    const TCHAR* pcValue,
                    bool*        pbResult,
                    bool         bDefault )
{
    DWORD nResultSize = sizeof( DWORD );
    DWORD nResult;

    LONG lResult = RegQueryValueEx(
        key,
        pcValue,                              // value to query
        NULL,                                 // reserved
        NULL,                                 // tells you what type the value is
        reinterpret_cast<LPBYTE>( &nResult ), // store result here
        &nResultSize );

    if ( lResult == ERROR_FILE_NOT_FOUND )
    {
        // Create value if it wasn't found
        WriteBool( key, pcValue, bDefault );
        *pbResult = bDefault;
    }
    else if ( lResult != ERROR_SUCCESS )
    {
        ostringstream osErr;
        osErr << "Could not read registry value [" << pcValue << "]. " <<
            "System error: " << lResult;
        throw CRegistryException( osErr.str() );
    }
    else
    {
        *pbResult = nResult != 0;
    }
}

/******************************************************************************
QueryInt
******************************************************************************/
void
RegistryUtils::QueryInt(
                   HKEY         key,
                   const TCHAR* pcValue,
                   int*         pnResult,
                   int          nDefault )
{
    DWORD nResultSize = sizeof( DWORD );
    DWORD nResult;

    LONG lResult = RegQueryValueEx(
        key,
        pcValue,                              // value to query
        NULL,                                 // reserved
        NULL,                                 // tells you what type the value is
        reinterpret_cast<LPBYTE>( &nResult ), // store result here
        &nResultSize );

    if ( lResult == ERROR_FILE_NOT_FOUND )
    {
        // Create value if it wasn't found
        WriteInt( key, pcValue, nDefault );
        *pnResult = nDefault;
    }
    else if ( lResult != ERROR_SUCCESS )
    {
        ostringstream osErr;
        osErr << "Could not read registry value [" << pcValue << "]. " <<
            "System error: " << lResult;
        throw CRegistryException( osErr.str() );
    }
    else
    {
        *pnResult = static_cast<int>( nResult );
    }

}

/******************************************************************************
WriteString
******************************************************************************/
void
RegistryUtils::WriteString(
                           HKEY         key,
                           const WCHAR* pcValue,
                           const WCHAR* pcData )
{
    LONG lResult = RegSetValueExW(
        key,
        pcValue,
        0,          // reserved, must be 0
        REG_SZ,     // type string
        reinterpret_cast<LPBYTE>( const_cast<WCHAR *>( pcData ) ),
        static_cast<DWORD>( ( wcslen( pcData ) + 1 ) * sizeof( TCHAR ) ) );

    if ( lResult != ERROR_SUCCESS )
    {
        ostringstream osErr;
        osErr << "Could not write registry value [" << pcValue << "]. " <<
            "System error: " << lResult;
        throw CRegistryException( osErr.str() );
    }
}


void
RegistryUtils::WriteString(
                           HKEY         key,
                           const char* pcValue,
                           const char* pcData )
{
    LONG lResult = RegSetValueExA(
        key,
        pcValue,
        0,          // reserved, must be 0
        REG_SZ,     // type string
        reinterpret_cast<LPBYTE>( const_cast<char *>( pcData ) ),
        static_cast<DWORD>( ( strlen( pcData ) + 1 ) * sizeof( char ) ) );

    if ( lResult != ERROR_SUCCESS )
    {
        ostringstream osErr;
        osErr << "Could not write registry value [" << pcValue << "]. " <<
            "System error: " << lResult;
        throw CRegistryException( osErr.str() );
    }
}

/******************************************************************************
WriteBool
******************************************************************************/
void
RegistryUtils::WriteBool(
                         HKEY         key,
                         const char* pcValue,
                         bool         bData )
{
    DWORD dWordSize = sizeof( DWORD );
    DWORD nValue = bData ? 1 : 0;

    LONG lResult = RegSetValueExA(
        key,
        pcValue,
        0,          // reserved, must be 0
        REG_DWORD,  // type DWORD
        reinterpret_cast<LPBYTE>( &nValue ),
        dWordSize );

    if ( lResult != ERROR_SUCCESS )
    {
        ostringstream osErr;
        osErr << "Could not write registry value [" << pcValue << "]. " <<
            "System error: " << lResult;
        throw CRegistryException( osErr.str() );
    }
}

void
RegistryUtils::WriteBool(
                    HKEY         key,
                    const WCHAR* pcValue,
                    bool         bData )
{
    DWORD dWordSize = sizeof( DWORD );
    DWORD nValue = bData ? 1 : 0;

    LONG lResult = RegSetValueExW(
        key,
        pcValue,
        0,          // reserved, must be 0
        REG_DWORD,  // type DWORD
        reinterpret_cast<LPBYTE>( &nValue ),
        dWordSize );

    if ( lResult != ERROR_SUCCESS )
    {
        ostringstream osErr;
        osErr << "Could not write registry value [" << pcValue << "]. " <<
            "System error: " << lResult;
        throw CRegistryException( osErr.str() );
    }
}

/******************************************************************************
WriteInt
******************************************************************************/
void
RegistryUtils::WriteInt(
                   HKEY         key,
                   const TCHAR* pcValue,
                   int          nData)
{
    DWORD dWordSize = sizeof( DWORD );

    LONG lResult = RegSetValueEx(
        key,
        pcValue,
        0,          // reserved, must be 0
        REG_DWORD,  // type DWORD
        reinterpret_cast<LPBYTE>( &nData ),
        dWordSize );

    if (lResult != ERROR_SUCCESS)
    {
        ostringstream osErr;
        osErr << "Could not write registry value [" << pcValue << "]. " <<
            "System error: " << lResult;
        throw CRegistryException( osErr.str() );
    }
}

/******************************************************************************
CloseRegKey
******************************************************************************/
void
RegistryUtils::CloseRegKey(
                      HKEY key )
{
    LONG lResult = RegCloseKey( key );

    if ( lResult != ERROR_SUCCESS )
    {
        ostringstream osErr;
        osErr << "Could not close registry key. " <<
            "System error: " << lResult;
        throw CRegistryException( osErr.str() );
    }
}

void 
RegistryUtils::DeleteValue( HKEY key, LPCWSTR pcValue)
{
    LONG lResult = RegDeleteValueW( key, pcValue );

    if( lResult != ERROR_SUCCESS )
    {
        ostringstream osErr;
        osErr << "Could not delete registry value. " <<
            "System error: " << lResult;

        throw CRegistryException( osErr.str() );
    }
}
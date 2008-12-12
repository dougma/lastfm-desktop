/* Copyright 205-2009, Last.fm Ltd. <client@last.fm>                       
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY <copyright holder> ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _REGISTRY_UTILS_H_
#define _REGISTRY_UTILS_H_

#include "StdString.h"

#include <windows.h>

#include <string>
#include <vector>

/*************************************************************************/ /**
A bunch of static registry utility functions. Incomplete.
******************************************************************************/
class RegistryUtils
{
public:

    class CRegistryException : public std::logic_error {
    public:
        CRegistryException(const std::string& sMsg) : logic_error(sMsg) { }
    };

    /*********************************************************************/ /**
    Get a key handle by passing in a parent key (HKEY_CURRENT_USER,
    HKEY_LOCAL_MACHINE) and a subkey ("Software\Last.fm\Service\1.0").
    **************************************************************************/
    static HKEY
        GetKey(
        HKEY         parentKey,
        const WCHAR* pcSubkey,
        bool         bCreateIfNotFound = false);

    static HKEY
        GetKey(
        HKEY         parentKey,
        const char*  pcSubkey,
        bool         bCreateIfNotFound = false);

    /*********************************************************************/ /**
    Returns the number of subkeys for the key identified by key.
    **************************************************************************/
    static int
        GetNumSubkeys(
        HKEY key);

    /*********************************************************************/ /**
    Reads the names of all subkeys of key into vector vNames.
    **************************************************************************/
    static void
        GetSubkeyNames(
        HKEY                  key,
        std::vector<tstring>& vNames);

    /*********************************************************************/ /**
    Returns the size of the longest subkey name in TCHARs.
    **************************************************************************/
    static DWORD
        GetLongestSubkeySize(
        HKEY key);

    /*********************************************************************/ /**
    Returns the size of the longest data entry in bytes.
    **************************************************************************/
    static DWORD
        GetLongestDataSize(
        HKEY key);

    /*********************************************************************/ /**
    Fills pcResult with the string stored in the registry under key key
    and value pcValue.
    **************************************************************************/

    // FIXME: do we still need both? Notice that it only differs on pcValue
    // and pcDefault... Should they really be char*s?
    static void
        QueryString(
        HKEY         key,
        const char* pcValue,
        char*       pcResult,
        DWORD        nResultSize,
        bool         bCreateIfNotFound = true,
        const char* pcDefault = "" );

    static void
        QueryString(
        HKEY         key,
        const WCHAR* pcValue,
        WCHAR*       pcResult,
        DWORD        nResultSize,
        bool         bCreateIfNotFound = true,
        const WCHAR* pcDefault = L"" );

    /*********************************************************************/ /**
        Fills pbResult with the bool stored in the registry under key key
        and value pcValue.
    **************************************************************************/
    static void
        QueryBool(
        HKEY         key,
        const TCHAR* pcValue,
        bool*        pbResult,
        bool         bDefault = false);

    /*********************************************************************/ /**
        Fills pnResult with the int stored in the registry under key key
        and value pcValue.
        **************************************************************************/
    static void
        QueryInt(
        HKEY         key,
        const TCHAR* pcValue,
        int*         pnResult,
        int          nDefault = 0);

    /*********************************************************************/ /**
        Writes the string pcData to the value named by pcValue in the key key.
        Creates the value if it doesn't already exist.
        **************************************************************************/

    // FIXME: need all these?
    static void
        WriteString(
        HKEY         key,
        const char* pcValue,
        const char* pcData);

    static void
        WriteString(
        HKEY         key,
        const WCHAR* pcValue,
        const WCHAR* pcData);

    static void
        DeleteValue(
        HKEY key,
        LPCWSTR pcValue);

    /*********************************************************************/ /**
        Writes the bool bData to the value named by pcValue in the key key.
        Creates the value if it doesn't already exist.
        **************************************************************************/
    static void
        WriteBool(
        HKEY         key,
        const char* pcValue,
        bool         bData);

    static void
        WriteBool(
        HKEY         key,
        const WCHAR* pcValue,
        bool         bData);

    /*********************************************************************/ /**
    Writes the int nData to the value named by pcValue in the key key.
    Creates the value if it doesn't already exist.
    **************************************************************************/
    static void
        WriteInt(
        HKEY         key,
        const TCHAR* pcValue,
        int          nData);

    /*********************************************************************/ /**
    Closes the registry key handed to function.
    **************************************************************************/
    static void
        CloseRegKey(
        HKEY key);

    static void
        DeleteRegKey(
        HKEY key);

private:

    /*********************************************************************/ /**
    Used to cache the info about a registry key returned by
    ReqQueryInfoKey.
    **************************************************************************/
    class Registry
    {
    public:
        HKEY    mKey;
        int     mnNumSubkeys;
        size_t  mnSizeLongestSubkey;
        int     mnNumValues;
        size_t  mnSizeLongestValue;
        size_t  mnSizeLongestData;
    };

    /*********************************************************************/ /**
    Fills the info cache by calling the Win32 API function RegQueryInfoKey.
    **************************************************************************/
    static void
        QueryRegInfo(
        HKEY key);


    // Last returned key info
    static Registry            mRegInfo;

};

#endif //_REGISTRY_UTILS_H_
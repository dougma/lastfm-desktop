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

#include "EncodingUtils.h"
#include <errno.h>

#include <string>

#ifdef WIN32
    #define POINTERINC( X ) X++
#else
    #define POINTERINC( X ) X+= sizeof( *X )
#endif

using namespace std;

/******************************************************************************
    AnsiToUtf8
******************************************************************************/
int
EncodingUtils::AnsiToUtf8(
    const char* ansi,
    char*       utf8,
    int         nUtf8Size)
{
    WCHAR*  wszValue;          // Unicode value
    size_t  ansi_len;
    size_t  len;

    if (ansi == NULL)
    {
        return 0;
    }

    ansi_len = strlen(ansi);

    // Malloc enough memory for a Unicode string the size of the ansi string
    wszValue = (WCHAR *)malloc((ansi_len + 1) * 2);
    if (wszValue == NULL)
    {
        return 0;
    }
    
    // Convert ANSI string to Unicode
    
    #ifdef WIN32
        len = MultiByteToWideChar(CP_ACP,
                                  0,
                                  ansi,
                                  static_cast<int>(ansi_len + 1),
                                  wszValue,
                                  static_cast<int>((ansi_len + 1) * 2 ));
    #else
        // This will use the current system locale (probably C)
        len = mbstowcs( wszValue, ansi, ansi_len + 1 );
    #endif

    if ( len == 0 )
    {
        free(wszValue);
        return 0;
    }

    // Convert Unicode value to UTF-8
    *utf8 = '\0';
    len = UnicodeToUtf8(wszValue, -1, utf8, nUtf8Size);
    if (len == 0 )
    {
        free(wszValue);
        return 0;
    }

    free(wszValue);

    return static_cast<int>(len-1);
}

/******************************************************************************
    UnicodeToUtf8
******************************************************************************/
int
EncodingUtils::UnicodeToUtf8(
    const WCHAR* lpWideCharStr,
    int          cwcChars,
    char*        lpUtf8Str,
    int          nUtf8Size)
{
    const unsigned short*   pwc = (unsigned short *)lpWideCharStr;
    unsigned char*          pmb = (unsigned char  *)lpUtf8Str;
    const unsigned short*   pwce; // ptr to end of wide string
    size_t  cBytes = 2;

    if ( cwcChars >= 0 ) {
        pwce = pwc + ( cwcChars * sizeof( *pwc )) ;
    } else {
        pwce = (unsigned short *)((size_t)-1);
    }

    while ( pwc < pwce ) {
        unsigned short  wc = *pwc;
        POINTERINC( pwc );

        if ( wc < 0x00000080 )
        {
            *pmb = (char)wc;
            POINTERINC( pmb );
            cBytes++;
        }
        else if ( wc < 0x00000800 )
        {
            *pmb = (char)(0xC0 | ((wc >>  6) & 0x1F));
            POINTERINC( pmb );
            cBytes++;
            *pmb = (char)(0x80 |  (wc        & 0x3F));
            POINTERINC( pmb );
            cBytes++;
        }
        else if ( wc < 0x00010000 )
        {
            *pmb = (char)(0xE0 | ((wc >> 12) & 0x0F));
            POINTERINC( pmb );
            cBytes++;
            *pmb = (char)(0x80 | ((wc >>  6) & 0x3F));
            POINTERINC( pmb );
            cBytes++;
            *pmb = (char)(0x80 |  (wc        & 0x3F));
            POINTERINC( pmb );
            cBytes++;
        }

        // Reached the end?
        if (wc == L'\0' || cBytes >= (size_t)(nUtf8Size - 3))
        {
            *pmb = '\0';
            return static_cast<int>(cBytes);
        }
    }

    return static_cast<int>(cBytes);
}


/******************************************************************************
    Utf8ToAnsi
******************************************************************************/
string
EncodingUtils::Utf8ToAnsi(
    const char* pcUTF8Str)
{
    // So, we'll do:
    // UTF8 Multibyte to Unicode
    // Unicode to ANSI Multibyte
    
    if (strlen(pcUTF8Str) == 0)
    {
        return string();
    }

    WCHAR* pcWideStr = NULL;
    char*  pcAnsiStr = NULL;

    // Get number of (wide or ANSI) chars the UTF8 string corresponds to
    // TODO: check why we aren't logging the errors!
    #ifdef WIN32
        int nLength = MultiByteToWideChar(CP_UTF8, 0, pcUTF8Str, -1, pcWideStr, 0);

        if (nLength == 0)
        {
            int nError = GetLastError();
            
            // ERROR_INSUFFICIENT_BUFFER
            // ERROR_INVALID_FLAGS
            // ERROR_INVALID_PARAMETER
            // ERROR_NO_UNICODE_TRANSLATION
            //LOG(1, "Couldn't get length of UTF8 string: '" << pcUTF8Str <<
            //    "'. Returning empty string. Error code: " << nError << "\n");
            return string();
        }

    #else
        char* curLocale = setlocale( LC_CTYPE, NULL );
        setlocale( LC_CTYPE, "UTF-8"  );
        int nLength = mbstowcs( NULL, pcUTF8Str, 0 );
        setlocale( LC_CTYPE, curLocale );
    
        if( nLength == (size_t) -1 )
        {
            int nError = errno;

            return string();
        }
    
    #endif

    // Allocate enough space for the wide version
    pcWideStr = new WCHAR[nLength + 1]; // length is in characters, not bytes

    #ifdef WIN32
        int nStatus = MultiByteToWideChar(CP_UTF8, 0, pcUTF8Str, -1, pcWideStr, nLength);

        if (nStatus == 0)
        {
            int nError = GetLastError();
            //LOG(1, "Couldn't convert UTF8 string '" << pcUTF8Str <<
            //    "' to Unicode. Returning empty string. Error code: " << nError << "\n");
            delete[] pcWideStr;
            return string();
        }

    #else
        setlocale( LC_CTYPE, "UTF-8" );
        int nStatus = mbstowcs( pcWideStr, pcUTF8Str, nLength + 1);
        setlocale( LC_CTYPE, curLocale );

        if (nStatus == (size_t) -1 )
        {
            delete[] pcWideStr;
            return string();
        }
    #endif

    // Allocate enough space for the ANSI char* version
    pcAnsiStr = new char[nLength + 5]; // 5?

    // Conversion to ANSI (CP_ACP)
    #ifdef WIN32
        nStatus = WideCharToMultiByte(CP_ACP, 0, pcWideStr, -1, pcAnsiStr, nLength, NULL, NULL);
        
        if (nStatus == 0)
        {
            int nError = GetLastError();

            // ERROR_INSUFFICIENT_BUFFER
            // ERROR_INVALID_FLAGS
            // ERROR_INVALID_PARAMETER
            //LOG(1, "Couldn't convert Unicode string '" << pcWideStr << "' to ANSI." <<
            //   " Error code: " << nError << "\n");
            delete[] pcWideStr;
            delete[] pcAnsiStr;
            return string();
        }
    #else
        nStatus = wcstombs( pcAnsiStr, pcWideStr, nLength + 5 );

        if( nStatus == (size_t)-1 )
        {
            delete[] pcWideStr;
            delete[] pcAnsiStr;
            return string();
        }
    #endif
   
    pcAnsiStr[nLength] = 0;

    string sAnsi(pcAnsiStr);
    
    delete[] pcWideStr;
    delete[] pcAnsiStr;

    return sAnsi;
}

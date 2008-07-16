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

#ifdef __APPLE__
    #include "client/mac/handler/exception_handler.h"
    #define HANDLER_ALL 1
    #define CRASH_REPORTER_BINARY "CrashReporter"
#elif defined WIN32
    #include "client/windows/handler/exception_handler.h"
    #define CRASH_REPORTER_BINARY L"CrashReporter.exe"
#elif defined __linux__
    #include "client/linux/handler/exception_handler.h"
    #define HANDLER_ALL 1
    #define CRASH_REPORTER_BINARY "last.breakpad"
#endif


#ifndef WIN32
#include <unistd.h>

static bool
execUploader( const char* dump_dir,
              const char* minidump_id,
              void* that, 
              bool succeeded )
{
    if (!succeeded)
        return false;

    // DON'T USE THE HEAP!!!
    // So that indeed means, no QStrings, no qDebug(), no QAnything, 
    // no std::anything! Seriously!

    pid_t pid = fork();

    if (pid == -1) // fork failed
        return false;
    if (pid == 0) { // we are the fork
        execl( CRASH_REPORTER_BINARY,
               CRASH_REPORTER_BINARY,
               dump_dir,
               minidump_id,
               static_cast<BreakPad*>(that)->productName(),
               (char*) 0 );

        // execl replaces this process, so no more code will be executed
        // unless it failed. If it failed, then we should return false.
        return false;
    }

    // we called fork()
    return true;
}


#else

static bool
execUploader( const wchar_t* dump_dir,
              const wchar_t* minidump_id,
              void* that,
              EXCEPTION_POINTERS *exinfo,
              MDRawAssertionInfo *assertion,
              bool succeeded )
{
    if (!succeeded)
        return false;

    // DON'T USE THE HEAP!!!
    // So that indeed means, no QStrings, no qDebug(), no QAnything, 
    // no std::anything! Seriously!

    const char* m_product_name = static_cast<BreakPad*>(that)->productName();

    // convert m_product_name to widechars, which sadly means the product name must be Latin1    
    wchar_t product_name[ 256 ];
    char* out = (char*)product_name;
    const char* in = m_product_name - 1;
    do {
        *out++ = *++in; //latin1 chars fit in first byte of each wchar
        *out++ = '\0';  //every second byte is NULL
    }
    while (*in);

    wchar_t command[MAX_PATH * 3 + 6];
    wcscpy( command, CRASH_REPORTER_BINARY L" \"" );
    wcscat( command, dump_dir );
    wcscat( command, L"\" \"" );
    wcscat( command, minidump_id );
    wcscat( command, L"\" \"" );
    wcscat( command, product_name );
    wcscat( command, L"\"" );

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWNORMAL;
    ZeroMemory( &pi, sizeof(pi) );

    if (CreateProcess( NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        CloseHandle( pi.hProcess );
        CloseHandle( pi.hThread );
        TerminateProcess( GetCurrentProcess(), 1 );
    }

    return false;
}

#endif // WIN32

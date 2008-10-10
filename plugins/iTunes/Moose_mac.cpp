/***************************************************************************
 *   Copyright 2008 Last.fm Ltd. <client@last.fm>                          *
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

#include "Moose.h"
#include "logger.h"

#include <sys/stat.h>
#include <sys/sysctl.h>

#include <CoreServices/CoreServices.h>
#include <CoreFoundation/CoreFoundation.h>


std::string
Moose::applicationSupport()
{
    std::string path = std::getenv( "HOME" );
    path += "/Library/Application Support/Last.fm/";
    return path;
}


static std::string
CFStringToStdString( CFStringRef s )
{
    std::string r;

    if (s == NULL)
        return r;

    CFIndex n;
    n = CFStringGetLength( s );
    n = CFStringGetMaximumSizeForEncoding( n, kCFStringEncodingUTF8 );
    char* buffer = new char[n];
    CFStringGetCString( s, buffer, n, kCFStringEncodingUTF8 );

    r = buffer;
    delete[] buffer;
    return r;
}


std::string
Moose::applicationPath()
{
    CFStringRef path = (CFStringRef) CFPreferencesCopyAppValue(
            CFSTR( "Path" ),
            CFSTR( MOOSE_PREFS_PLIST ) );

    if ( path == NULL )
        return "/Applications/Last.fm.app/Contents/MacOS/Last.fm";

    std::string s = CFStringToStdString( path );
    CFRelease( path );
    return s;
}


bool
Moose::iPodScrobblingEnabled()
{
    Boolean key_exists;
    bool b = CFPreferencesGetAppBooleanValue( 
                    CFSTR( "iPodScrobblingEnabled"),
                    CFSTR( MOOSE_PREFS_PLIST ),
                    &key_exists );
                    
    if (!key_exists) return true;

    return b;
}


std::string
Moose::applicationFolder()
{
    std::string s = applicationPath();
    return s.substr( 0, s.rfind( '/' ) + 1 );
}


bool
Moose::launchWithMediaPlayer()
{
    CFBooleanRef v = (CFBooleanRef) CFPreferencesCopyAppValue( 
            CFSTR( "LaunchWithMediaPlayer" ), 
            CFSTR( MOOSE_PREFS_PLIST ) );

    if (v)
    {
        bool b = CFBooleanGetValue( v );
        CFRelease( v );
        return b;
    }
    else
        return true;
}


void
Moose::setFileDescriptorsCloseOnExec()
{
    int n = 0;
    int fd = sysconf( _SC_OPEN_MAX );
    while (--fd > 2)
    {
        int flags = fcntl( fd, F_GETFD, 0 );
        if ((flags != -1) && !(flags & FD_CLOEXEC))
        {
            n++;
            flags |= FD_CLOEXEC;
            fcntl( fd, F_SETFD, flags );
        }
    }
    if (n) LOGL( 3, "Set " << n << " file descriptors FD_CLOEXEC" );
}


bool
Moose::exec( const std::string& command, const std::string& args )
{
    // fixes error -54 bug, where endless dialogs are spawned
    // presumably because the child Twiddly process inherits some key
    // file descriptors from iTunes
    setFileDescriptorsCloseOnExec();
    
    std::string s = "\"" + command + "\" " + args + " &";

    LOGL( 3, "Launching `" << s << "'" )
    return ( std::system( s.c_str() ) >= 0 );
}


/** Returns a list of all BSD processes on the system.  This routine
  * allocates the list and puts it in *procList and a count of the
  * number of entries in *procCount.  You are responsible for freeing
  * this list (use "free" from System framework).
  * On success, the function returns 0.
  * On error, the function returns a BSD errno value. 
  */
static int
getBsdProcessList( kinfo_proc **procList, size_t *procCount )
{
    int                 err;
    kinfo_proc *        result;
    bool                done;
    static const int    name[] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };
    // Declaring name as const requires us to cast it when passing it to
    // sysctl because the prototype doesn't include the const modifier.
    size_t              length;

    // We start by calling sysctl with result == NULL and length == 0.
    // That will succeed, and set length to the appropriate length.
    // We then allocate a buffer of that size and call sysctl again
    // with that buffer.  If that succeeds, we're done.  If that fails
    // with ENOMEM, we have to throw away our buffer and loop.  Note
    // that the loop causes use to call sysctl with NULL again; this
    // is necessary because the ENOMEM failure case sets length to
    // the amount of data returned, not the amount of data that
    // could have been returned.
    result = NULL;
    done = false;
    *procCount = 0;

    do
    {
        // Call sysctl with a NULL buffer.
        length = 0;
        err = sysctl( (int *) name, ( sizeof( name ) / sizeof( *name ) ) - 1, NULL, &length, NULL, 0 );
        if (err == -1)
        {
            err = errno;
        }

        // Allocate an appropriately sized buffer based on the results
        // from the previous call.
        if ( err == 0 )
        {
            result = (kinfo_proc*)malloc( length );
            if ( result == NULL )
            {
                err = ENOMEM;
            }
        }

        // Call sysctl again with the new buffer.  If we get an ENOMEM
        // error, toss away our buffer and start again.
        if ( err == 0 )
        {
            err = sysctl( (int *) name, ( sizeof( name ) / sizeof( *name ) ) - 1, result, &length, NULL, 0 );
            if ( err == -1 )
            {
                err = errno;
            }
            if (err == 0)
            {
                done = true;
            } else if ( err == ENOMEM )
            {
                free( result );
                result = NULL;
                err = 0;
            }
        }
    } while ( err == 0 && !done );

    // Clean up and establish post conditions.
    if ( err != 0 && result != NULL )
    {
        free( result );
        result = NULL;
    }

    *procList = result;
    if ( err == 0 )
    {
        *procCount = length / sizeof( kinfo_proc );
    }

    return err;
}


bool
Moose::isTwiddlyRunning()
{
    bool found = false;
    
    kinfo_proc* processList = NULL;
    size_t processCount = 0;

    if ( getBsdProcessList( &processList, &processCount ) )
    {
        LOGL( 3, "Failed to get the process list" );
        return false;
    }

    uint const uid = ::getuid();
    for ( size_t processIndex = 0; processIndex < processCount; processIndex++ )
    {
        if ( processList[processIndex].kp_eproc.e_pcred.p_ruid == uid )
        {
            if ( strcmp( processList[processIndex].kp_proc.p_comm, 
                         TWIDDLY_EXECUTABLE_NAME ) == 0 )
            {
                found = true;
                break;
            }
        }
    }

    free( processList );

    if ( found ) LOGL( 3, "Twiddly already running!" );

    return found;
}


std::string
Moose::bundleFolder()
{
    std::string path;
    path += ::getenv( "HOME" );
    path += "/Library/iTunes/iTunes Plug-ins/AudioScrobbler.bundle/";
    return path;
}


std::string
Moose::pluginPath()
{
    return bundleFolder() + "Contents/MacOS/AudioScrobbler";
}

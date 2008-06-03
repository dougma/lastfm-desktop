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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include "UnicornCommonMac.h"

#include "AppleScript.h"

#include <QDebug>
#include <QDir>
#include <Carbon/Carbon.h>

#include <sys/sysctl.h>


QString
UnicornUtils::applicationSupportFolderPath()
{
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
    return QDir::homePath() + QString::fromStdString( outString );
}


QLocale::Language
UnicornUtils::osxLanguageCode()
{
    CFArrayRef languages;
    languages = (CFArrayRef)CFPreferencesCopyValue( CFSTR( "AppleLanguages" ),
                                                    kCFPreferencesAnyApplication,
                                                    kCFPreferencesCurrentUser,
                                                    kCFPreferencesAnyHost );

    QString langCode;

    if ( languages != NULL )
    {
        CFStringRef uxstylelangs = CFStringCreateByCombiningStrings( kCFAllocatorDefault, languages, CFSTR( ":" ) );
        langCode = UnicornUtils::CFStringToQString( uxstylelangs ).split( ':' ).value( 0 );
    }

    return QLocale( langCode ).language();
}


QByteArray
UnicornUtils::CFStringToUtf8( CFStringRef s )
{
    QByteArray result;

    if (s != NULL) 
    {
        CFIndex length;
        length = CFStringGetLength( s );
        length = CFStringGetMaximumSizeForEncoding( length, kCFStringEncodingUTF8 ) + 1;
        char* buffer = new char[length];

        if (CFStringGetCString( s, buffer, length, kCFStringEncodingUTF8 ))
            result = QByteArray( buffer );
        else
            qWarning() << "CFString conversion failed.";

        delete[] buffer;
    }

    return result;
}


/** Returns a list of all BSD processes on the system.  This routine
  * allocates the list and puts it in *procList and a count of the
  * number of entries in *procCount.  You are responsible for freeing
  * this list (use "free" from System framework).
  * On success, the function returns 0.
  * On error, the function returns a BSD errno value. 
  */
int
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
isProcessRunning( const QString& processName )
{
    bool found = false;
    
    kinfo_proc* processList = NULL;
    size_t processCount = 0;

    if ( getBsdProcessList( &processList, &processCount ) )
    {
        return false;
    }

    uint const uid = ::getuid();
    for ( size_t processIndex = 0; processIndex < processCount; processIndex++ )
    {
        if ( processList[processIndex].kp_eproc.e_pcred.p_ruid == uid )
        {
            if ( strcmp( processList[processIndex].kp_proc.p_comm, 
                         processName.toLocal8Bit() ) == 0 )
            {
                found = true;
                break;
            }
        }
    }

    free( processList );
    return found;
}


bool
UnicornUtils::isGrowlInstalled()
{
    return isProcessRunning( "GrowlHelperApp" );
}


bool
UnicornUtils::iTunesIsOpen()
{
    return isProcessRunning( "iTunes" );
}


bool
UnicornUtils::setPreferredAppForUrlScheme( const QUrl& url, const QString& app )
{
    QString scheme = url.scheme();
    CFStringRef urlStr = CFStringCreateWithCharacters( NULL, reinterpret_cast<const UniChar *>( scheme.unicode() ), scheme.length() );
    CFStringRef bundleStr = CFStringCreateWithCharacters( NULL, reinterpret_cast<const UniChar *>( app.unicode() ), app.length() );

    CFURLRef bundleURL;
    CFBundleRef bundleRef;
    bundleURL = CFURLCreateWithFileSystemPath( kCFAllocatorDefault, bundleStr, kCFURLPOSIXPathStyle, true );
    bundleRef = CFBundleCreate( kCFAllocatorDefault, bundleURL );

    CFStringRef appIdentifier = CFBundleGetIdentifier( bundleRef );
    OSStatus err = LSSetDefaultHandlerForURLScheme( urlStr, appIdentifier );

    CFRelease( urlStr );
    CFRelease( bundleStr );
    CFRelease( bundleURL );
    CFRelease( bundleRef );

    return ( err == noErr );
}


QString
UnicornUtils::preferredAppForUrlScheme( const QUrl& url )
{
    QString app;
    QString scheme = url.scheme() + "://";

    CFStringRef urlStr = CFStringCreateWithCharacters( NULL, reinterpret_cast<const UniChar *>( scheme.unicode() ), scheme.length() );
    CFURLRef urlRef = CFURLCreateWithString( NULL, urlStr, NULL );

    FSRef appRef;
    OSStatus err = LSGetApplicationForURL( urlRef, kLSRolesAll, &appRef, NULL );

    if ( err == noErr )
    {
        unsigned char path[512];
        err = ::FSRefMakePath( &appRef, path, 512 );

        if ( err == noErr )
            app = (const char*)path;
    }

    CFRelease( urlStr );
    CFRelease( urlRef );

    return QDir::cleanPath( app );
}

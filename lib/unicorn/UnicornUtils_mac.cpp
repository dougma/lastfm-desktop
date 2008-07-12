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

#include "UnicornUtils.h"
#include "AppleScript.h"
#include <QDebug>
#include <QDir>
#include <Carbon/Carbon.h>


QLocale::Language
Unicorn::osxLanguageCode()
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
        langCode = Unicorn::CFStringToQString( uxstylelangs ).split( ':' ).value( 0 );
    }

    return QLocale( langCode ).language();
}


QByteArray
Unicorn::CFStringToUtf8( CFStringRef s )
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


#include "common/c++/mac/getBsdProcessList.c"

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
Unicorn::isGrowlInstalled()
{
    return isProcessRunning( "GrowlHelperApp" );
}


bool
Unicorn::iTunesIsOpen()
{
    return isProcessRunning( "iTunes" );
}

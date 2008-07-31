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

#ifndef UNICORN_COMMON_H
#define UNICORN_COMMON_H

#include "lib/DllExportMacro.h"
#include <QString>
#include <QLocale>
#include <QUrl>

//FIXME sucks
#ifdef WIN32
#include "windows.h"
#endif
#ifdef Q_WS_MAC
#include <Carbon/Carbon.h>
#endif


/** A very arbitrary bunch of utility functions gathered from various legacy
  * classes. Intended to be used across applications so should have no
  * dependencies on the moose layer.
  *
  * @author <erik@last.fm> 
  */
namespace Unicorn
{
    /** Translate a Qt language code into the language code used on the Last.fm 
      * site */
    UNICORN_DLLEXPORT QString qtLanguageToLfmLangCode( QLocale::Language qtLang );

    /** Translate a Last.fm site language code into the equivalent ISO 639
      * language code as used in HTTP headers */
    UNICORN_DLLEXPORT QString lfmLangCodeToIso639( const QString& code );

    /** Translate a Last.fm site language code into the equivalent hostname */
    UNICORN_DLLEXPORT QString localizedHostName( const QString& code );

    /** Use this to URL encode any database item (artist, track, album). It
      * internally calls UrlEncodeSpecialChars to double encode some special
      * symbols according to the same pattern as that used on the website.
      *
      * &, /, ;, +, #
      *
      * Use for any urls that go to www.last.fm
      * Do not use for ws.audioscrobbler.com
      *
      * @param[in] str String to encode.
      */
    UNICORN_DLLEXPORT QString urlEncodeItem( QString item );
    UNICORN_DLLEXPORT QString urlDecodeItem( QString item );

    /** QStringList::sort() sorts with uppercase first */
    UNICORN_DLLEXPORT QStringList sortCaseInsensitively( QStringList input );

    UNICORN_DLLEXPORT QString verbosePlatformString();

    UNICORN_DLLEXPORT void msleep( int );
    
    /** @returns information about the system. operating system, CPU, 
      * memory, diskspace */ //TODO not in unicorn
    UNICORN_DLLEXPORT QString systemInformation();
    
    /** Runs a shell command, waits for the process to finish then return the output */
    UNICORN_DLLEXPORT QString runCommand( const QString& );

#ifdef WIN32
    /** @returns true if we're running on a limited user account */
    UNICORN_DLLEXPORT bool isLimitedUser();

    /** @returns the name of the default player. "" if not found */
    UNICORN_DLLEXPORT QString findDefaultPlayer();

    /** Function......: CreateShortcut
      * Parameters....: lpszFileName - string that specifies a valid file name
      *                 lpszDesc - string that specifies a description for a 
      *                            shortcut
      *                 lpszShortcutPath - string that specifies a path and 
      *                                    file name of a shortcut
      * Returns.......: S_OK on success, error code on failure
      * Description...: Creates a Shell link object (shortcut)
      */
    UNICORN_DLLEXPORT HRESULT createShortcut( /*in*/ LPCTSTR lpszFileName, 
                                              /*in*/ LPCTSTR lpszDesc, 
                                              /*in*/ LPCTSTR lpszShortcutPath );
#endif

#ifdef Q_WS_MAC
    QLocale::Language osxLanguageCode();
    QByteArray CFStringToUtf8( CFStringRef );
    inline QString CFStringToQString( CFStringRef s )
    {
        return QString::fromUtf8( CFStringToUtf8( s ) );
    }
    bool isProcessRunning( const QString& );
    inline bool iTunesIsOpen() 
    { 
        return isProcessRunning( "iTunes" ); 
    }
#endif
}

#endif // header guard

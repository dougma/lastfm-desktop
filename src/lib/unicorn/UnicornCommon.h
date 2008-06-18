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

#include "common/DllExportMacro.h"

#include <QString>
#include <QLocale>
#include <QUrl>

//TODO remove
#include <string>
#include <vector>

#ifdef Q_OS_WIN
    #include "UnicornCommonWin.h"
#endif
#ifdef Q_OS_MAC
    #include "UnicornCommonMac.h"
#endif


namespace UnicornEnums
{
    enum ItemType
    {
        ItemArtist = 1,
        ItemTrack,
        ItemAlbum,
        ItemTag,
        ItemUser,
        ItemStation,
        ItemUnknown
    };
}


/**
 * A very arbitrary bunch of utility functions gathered from various legacy
 * classes. Intended to be used across applications so should have no
 * dependencies on the moose layer.
 *
 * @author <erik@last.fm>
 */
namespace UnicornUtils
{
    DLLEXPORT QString
    md5( const QByteArray& );

    /**
     * Translate a QHttpState into a human-readable string.
     * TODO: move to Http class?
     */
    DLLEXPORT QString
    QHttpStateToString( int state );

    /**
     * Translate a Qt language code into the language code used on the Last.fm site.
     */
    DLLEXPORT QString
    qtLanguageToLfmLangCode( QLocale::Language qtLang );

    /**
     * Translate a Last.fm site language code into the equivalent ISO 639
     * language code as used in HTTP headers.
     */
    DLLEXPORT QString
    lfmLangCodeToIso639( const QString& code );

    /**
     * Translate a Last.fm site language code into the equivalent hostname.
     */
    DLLEXPORT QString
    localizedHostName( const QString& code );

    /**
     *   Takes a string containing several quoted strings, like
     *   "abc" "def" ...
     *   and separates the quoted strings out into the supplied vector.
     *
     *   @param[in] sCompound The compound string to be parsed.
     *   @param[out] separated The vector to put the individual strings in.
     *
     *   // WHY std::string!!
     *   // Because it's a legacy function from the MFC days.
     */
    DLLEXPORT void
    parseQuotedStrings( const std::string& sCompound,
                        std::vector<std::string>& separated );

    /**
     *   Trims string of whitespace at beginning and end.
     *
     *   @param[in] str String to trim.
     */
    DLLEXPORT void
    trim( std::string& str );

    /**
     *   Strips all []-enclosed sections from string.
     *
     *   @param[in] str String to strip.
     */
    DLLEXPORT void
    stripBBCode( std::string& str );

    DLLEXPORT void
    stripBBCode( QString& str );

    /**
     *   Use this to URL encode any database item (artist, track, album). It
     *   internally calls UrlEncodeSpecialChars to double encode some special
     *   symbols according to the same pattern as that used on the website.
     *
     *   Use for any urls that go to www.last.fm
     *   Do not use for ws.audioscrobbler.com
     *
     *   @param[in] str String to encode.
     */
    DLLEXPORT QString
    urlEncodeItem( QString item );

    DLLEXPORT QString
    urlDecodeItem( QString item );

    /**
     *   Encodes the following characters once to fit with the way artist
     *   names etc are encoded on the site: &, /, ;, +, #
     *
     *   The string returned from this function can then be passed to QUrl::
     *   toPercentEncoding.
     *
     *   Not exported because it's used internally by UrlEncodeItem.
     *
     *   @param[in] str String to encode.
     */
    QString&
    urlEncodeSpecialChars( QString& str );

    QString&
    urlDecodeSpecialChars( QString& str );

    /**
     * QStringList::sort() sorts with uppercase first
     */
    DLLEXPORT QStringList
    sortCaseInsensitively( QStringList input );

    /**
     * Returns the path to the top-level Application Data folder.
     * Win XP: C:\Documents and Settings\user\Local Settings\Application Data.
     * Vista: C:\Users\user\AppData\Local
     * Mac: ~/Library/Application Support
     *
     * May return an empty string on Windows if the system call to get the
     * path fails.
     */
    DLLEXPORT QString
    appDataPath();

    DLLEXPORT QString
    getOSVersion();

    DLLEXPORT void
    msleep( int );
}

#endif

/***************************************************************************
 *   Copyright (C) 2005 - 2008 by                                          *
 *      Last.fm Ltd. <client@last.fm>                                      *
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

#ifndef UNICORN_COMMON_MAC_H
#define UNICORN_COMMON_MAC_H

/** @author <erik@last.fm> */

#include <QString>
#include <CoreFoundation/CoreFoundation.h>
#include <QLocale>
#include <QUrl>

namespace UnicornUtils
{
    /**
     * Returns the path to the user's Application Support directory.
     */
    QString
    applicationSupportFolderPath();

    QLocale::Language
    osxLanguageCode();
    
    QByteArray 
    CFStringToUtf8( CFStringRef );

    inline QString
    CFStringToQString( CFStringRef s )
    {
        return QString::fromUtf8( CFStringToUtf8( s ) );
    }
    
    bool
    isGrowlInstalled();

    bool
    iTunesIsOpen();

    bool
    setPreferredAppForUrlScheme( const QUrl& url, const QString& app );

    QString
    preferredAppForUrlScheme( const QUrl& url );
}

#endif // UNICORN_COMMON_MAC_H

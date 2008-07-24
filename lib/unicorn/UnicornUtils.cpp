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
#include "lib/core/Logger.h"
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QMap>
#include <QUrl>
#include <QProcess>
#ifdef WIN32
    #include <windows.h>
    #include <shlobj.h>
#endif



QString
Unicorn::qtLanguageToLfmLangCode( QLocale::Language qtLang )
{
    switch ( qtLang )
    {
        case QLocale::English:    return "en";
        case QLocale::French:     return "fr";
        case QLocale::Italian:    return "it";
        case QLocale::German:     return "de";
        case QLocale::Spanish:    return "es";
        case QLocale::Portuguese: return "pt";
        case QLocale::Polish:     return "pl";
        case QLocale::Russian:    return "ru";
        case QLocale::Japanese:   return "jp";
        case QLocale::Chinese:    return "cn";
        case QLocale::Swedish:    return "sv";
        case QLocale::Turkish:    return "tr";
        default:                  return "en";
    }
}


QString
Unicorn::lfmLangCodeToIso639( const QString& code )
{
    if ( code == "jp" ) return "ja";
    if ( code == "cn" ) return "zh";

    return code;
}


QString
Unicorn::localizedHostName( const QString& code )
{
    if ( code == "en" ) return "www.last.fm"; //first as optimisation
    if ( code == "pt" ) return "www.lastfm.com.br";
    if ( code == "tr" ) return "www.lastfm.com.tr";
    if ( code == "cn" ) return "cn.last.fm";
    if ( code == "sv" ) return "www.lastfm.se";

    QStringList simple_hosts = QStringList()
            << "fr" << "it" << "de" << "es" << "pl"
            << "ru" << "jp" << "se";

    if ( simple_hosts.contains( code ) )
        return "www.lastfm." + code;

    // else default to english site
    return "www.last.fm";
}


QString
Unicorn::urlEncodeItem( QString s )
{
    s.replace( "&", "%26" );
    s.replace( "/", "%2F" );
    s.replace( ";", "%3B" );
    s.replace( "+", "%2B" );
    s.replace( "#", "%23" );
    return QString::fromAscii( QUrl::toPercentEncoding( s ) );
}


QString
Unicorn::urlDecodeItem( QString s )
{
    s = QUrl::fromPercentEncoding( s.toAscii() );
    s.replace( "%26", "&" );
    s.replace( "%2F", "/" );
    s.replace( "%3B", ";" );
    s.replace( "%2B", "+" );
    s.replace( "%23", "#" );
    s.replace( '+', ' ' );
    return s;
}


QStringList
Unicorn::sortCaseInsensitively( QStringList input )
{
    // This cumbersome bit of code here is how the Qt docs suggests you sort
    // a string list case-insensitively
    QMap<QString, QString> map;
    foreach (QString s, input)
        map.insert( s.toLower(), s );

    QStringList output;
    QMapIterator<QString, QString> i( map );
    while (i.hasNext())
        output += i.next().value();

    return output;
}


QString
Unicorn::verbosePlatformString()
{
    #ifdef Q_WS_WIN
    switch (QSysInfo::WindowsVersion)
    {
        case QSysInfo::WV_32s:        return "Windows 3.1 with Win32s";
        case QSysInfo::WV_95:         return "Windows 95";
        case QSysInfo::WV_98:         return "Windows 98";
        case QSysInfo::WV_Me:         return "Windows Me";
        case QSysInfo::WV_DOS_based:  return "MS-DOS-based Windows";

        case QSysInfo::WV_NT:         return "Windows NT";
        case QSysInfo::WV_2000:       return "Windows 2000";
        case QSysInfo::WV_XP:         return "Windows XP";
        case QSysInfo::WV_2003:       return "Windows Server 2003";
        case QSysInfo::WV_VISTA:      return "Windows Vista";
        case QSysInfo::WV_NT_based:   return "NT-based Windows";

        case QSysInfo::WV_CE:         return "Windows CE";
        case QSysInfo::WV_CENET:      return "Windows CE.NET";
        case QSysInfo::WV_CE_based:   return "CE-based Windows";

        default:                      return "Unknown";
    }
    #elif defined Q_WS_MAC
    switch (QSysInfo::MacintoshVersion)
    {
        case QSysInfo::MV_Unknown:    return "Unknown Mac";
        case QSysInfo::MV_9:          return "Mac OS 9";
        case QSysInfo::MV_10_0:       return "Mac OS X 10.0";
        case QSysInfo::MV_10_1:       return "Mac OS X 10.1";
        case QSysInfo::MV_10_2:       return "Mac OS X 10.2";
        case QSysInfo::MV_10_3:       return "Mac OS X 10.3";
        case QSysInfo::MV_10_4:       return "Mac OS X 10.4";
        case QSysInfo::MV_10_5:       return "Mac OS X 10.5";
        
        default:                      return "Unknown";
    }
    #else
    return "Unix";
    #endif
}


void
Unicorn::msleep( int ms )
{
  #ifdef WIN32
    Sleep( ms );
  #else
    ::usleep( ms * 1000 );
  #endif
}


QString
Unicorn::runCommand( const QString& command )
{
    QProcess p;
    p.start( command );
    p.closeWriteChannel();
    p.waitForFinished();

    return QString( p.readAll() );
}

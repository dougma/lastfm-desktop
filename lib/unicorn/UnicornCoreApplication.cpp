/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
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

#include "UnicornCoreApplication.h"
#include "lib/lastfm/ws/WsKeys.h"
#include "lib/lastfm/core/CoreDir.h"
#include "lib/lastfm/core/CoreSettings.h"
#include "lib/lastfm/core/CoreSysInfo.h"
#include "common/c++/Logger.h"
#include <QDebug>
#include <QLocale>

#ifdef WIN32
extern void qWinMsgHandler( QtMsgType t, const char* msg );
#endif
#ifdef __APPLE__
static QLocale qMacLocale();
#endif


unicorn::CoreApplication::CoreApplication( int& argc, char** argv )
                      : QCoreApplication( argc, argv )
{
	init();
	
	CoreSettings s;
	Ws::Username = s.value( "Username" ).toString();
    Ws::SessionKey = s.value( "SessionKey" ).toString();
}


void //static
unicorn::CoreApplication::init()
{
	QCoreApplication::setOrganizationName( CoreSettings::organizationName() );
    QCoreApplication::setOrganizationDomain( CoreSettings::organizationDomain() );

    // OHAI! DON'T USE OURS! GET YOUR OWN! http://www.last.fm/api
    Ws::SharedSecret = "73582dfc9e556d307aead069af110ab8";
    Ws::ApiKey = "c8c7b163b11f92ef2d33ba6cd3c2c3c3";

	QVariant const v = CoreSettings().value( "Locale" );
    if (v.isValid())
        QLocale::setDefault( QLocale::Language(v.toInt()) );
#ifdef __APPLE__
    else
        QLocale::setDefault( qMacLocale() );
#endif

    CoreDir::mkpaths();

#ifdef WIN32
    QString bytes = log( applicationName() ).absoluteFilePath();
    const wchar_t* path = bytes.utf16();
#else
    QByteArray bytes = log( applicationName() ).absoluteFilePath().toLocal8Bit();
    const char* path = bytes.data();
#endif
    new Logger( path );

    qInstallMsgHandler( qMsgHandler );
    qDebug() << "Introducing" << applicationName()+' '+applicationVersion();
    qDebug() << "Directed by" << CoreSysInfo::platform();
}


void
unicorn::CoreApplication::qMsgHandler( QtMsgType type, const char* msg )
{
#ifndef NDEBUG
#ifdef WIN32
    qWinMsgHandler( type, msg );
#else
	Q_UNUSED( type );
    fprintf( stderr, "%s\n", msg );
    fflush( stderr );
#endif
#endif

    static int spam = 0;
    static QByteArray previous_msg;
    
    if (previous_msg == msg) {
        ++spam;
        return;
    }
    
    if (spam) {
        // +1 so as to include first duplication too
        Logger::the().log( QString( "Times above line spammed: %L1").arg( spam + 1 ).toUtf8() );
        spam = 0;
    }
    
    previous_msg = msg;    
    Logger::the().log( msg );
}


QFileInfo
unicorn::CoreApplication::log( const QString& productName )
{
#ifdef NDEBUG
    return CoreDir::logs().filePath( productName + ".log" );
#else
    return CoreDir::logs().filePath( productName + ".debug.log" );
#endif
}


#ifdef __APPLE__
#include "lib/lastfm/core/mac/CFStringToQString.h"
static QLocale qMacLocale()
{
	//TODO see what Qt's version does
	CFArrayRef languages = (CFArrayRef) CFPreferencesCopyValue( 
		    CFSTR( "AppleLanguages" ),
			kCFPreferencesAnyApplication,
			kCFPreferencesCurrentUser,
			kCFPreferencesAnyHost );
	
	if (languages == NULL)
		return QLocale::system();

	CFStringRef uxstylelangs = CFStringCreateByCombiningStrings( kCFAllocatorDefault, languages, CFSTR( ":" ) );

	QString const s = CFStringToQString( uxstylelangs ).split( ':' ).value( 0 );
	return QLocale( s );
}
#endif

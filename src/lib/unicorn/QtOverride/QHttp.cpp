/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd                                       *
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

#include "./QHttp"
#undef QHttp
#include "../Settings.h"
#include <QHttpRequestHeader>
#ifdef Q_WS_MAC
#include "../UnicornCommonMac.h"
#include <SystemConfiguration/SystemConfiguration.h> 
#endif


Unicorn::QHttp::QHttp( QObject* parent )
              : ::QHttp( parent )
{
    init();
}


Unicorn::QHttp::QHttp( const QString &hostname, quint16 port /* = 0 */, QObject *parent /* = 0 */ )
             : ::QHttp( hostname, port, parent ),
               m_host( hostname )
{
    init();
}


void
Unicorn::QHttp::init()
{
    Unicorn::Settings s;
    if (s.isUseProxy()) 
    {
        setProxy( s.proxyHost(), s.proxyPort(), s.proxyUser(), s.proxyPassword() );
    }
    else
    {
        applyAutoDetectedProxy();
    }
}


int 
Unicorn::QHttp::get( const QString& path )
{
    return request( QHttpRequestHeader( "GET", path ) );
}


int
Unicorn::QHttp::post( const QString& path, const QByteArray& data )
{
    return request( QHttpRequestHeader( "POST", path ), data );
}


int 
Unicorn::QHttp::request( QHttpRequestHeader header, const QByteArray& data )
{
    header.setValue( "Host", m_host );
    header.setValue( "Connection", "Keep-Alive" );

    // NEVER CHANGE THE FOLLOWING STRING! you can append stuff, but that's it
    QString agent = "Last.fm Client";
    //TODO agent = QCoreApplication::organizationName() + " " + QCoreApplication::applicationName();
#ifdef WIN32
    agent += " (Windows)";
#elif defined (Q_WS_MAC)
    agent += " (OS X)";
#elif defined (Q_WS_X11)
    agent += " (X11)";
#endif
    header.setValue( "User-Agent", agent );

    return ::QHttp::request( header, data );
}


void
Unicorn::QHttp::applyAutoDetectedProxy()
{
    static bool already_run = false;
    static QString host;
    static int port = -1;

    if (already_run)
    {
        if (port != -1)
            setProxy( host, port );
        return;
    }

#ifdef Q_WS_MAC
    CFNumberRef enableNum;
    int enable;

    // Get the dictionary.
    CFDictionaryRef proxyDict = SCDynamicStoreCopyProxies( NULL );
    bool result = (proxyDict != NULL);

    // Get the enable flag.  This isn't a CFBoolean, but a CFNumber.
    if (result) {
        enableNum = (CFNumberRef) CFDictionaryGetValue( proxyDict, kSCPropNetProxiesHTTPEnable );
        result = (enableNum != NULL) && (CFGetTypeID(enableNum) == CFNumberGetTypeID());
    }

    if (result)
        result = CFNumberGetValue( enableNum, kCFNumberIntType, &enable ) && (enable != 0);

    // get the proxy host
    CFStringRef hostStr;
    if (result) {
        hostStr = (CFStringRef) CFDictionaryGetValue( proxyDict, kSCPropNetProxiesHTTPProxy );
        result = (hostStr != NULL) && (CFGetTypeID(hostStr) == CFStringGetTypeID());
    }
    if (result)
        host = Unicorn::CFStringToQString( hostStr );

    // get the port
    int portInt;
    CFNumberRef portNum;
    if (result) {
        portNum = (CFNumberRef) CFDictionaryGetValue( proxyDict, kSCPropNetProxiesHTTPPort );
        result = (portNum != NULL) && (CFGetTypeID(portNum) == CFNumberGetTypeID());
    }
    if (result)
        result = CFNumberGetValue( portNum, kCFNumberIntType, &portInt );

    if (result)
        port = portInt;

    // clean up
    if (proxyDict != NULL)
        CFRelease( proxyDict );

#elif defined Q_WS_WIN
    QString const prefix = "CurrentVersion/Internet Settings/";
    ::QSettings autoProxySettings( QSettings::NativeFormat, QSettings::UserScope, "Microsoft", "Windows", this );

    bool const b = autoProxySettings.value( prefix + "ProxyEnable", "" ).toBool();

    if (b)
    {
        QString proxySettings = autoProxySettings.value( prefix + "ProxyServer", "" ).toString();
        QStringList proxySettingList = proxySettings.split( ":" );
        host = proxySettingList.value( 0 );
        port = proxySettingList.value( 1 ).toInt();
    }
#endif

    already_run = true;
}
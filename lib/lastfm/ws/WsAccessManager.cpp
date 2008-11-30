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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "WsAccessManager.h"
#include "WsConnectionMonitor.h"
#include "WsKeys.h"
#include <QCoreApplication>
#include <QNetworkRequest>
#ifdef WIN32
#include "win/IeSettings.h"
#include "win/Pac.h"
#endif
#ifdef __APPLE__
#include "mac/ProxyDict.h"
#endif


WsAccessManager::WsAccessManager(QObject *parent)
               : QNetworkAccessManager(parent)
            #ifdef WIN32
               , m_pac( 0 )
               , m_monitor( 0 )
            #endif
{
	if (!Ws::UserAgent)
		Ws::UserAgent = qstrdup(QCoreApplication::applicationName().toAscii()); //has to be latin1 I believe
}


WsAccessManager::~WsAccessManager()
{
#ifdef WIN32
    delete m_pac;
#endif
}


QNetworkProxy
WsAccessManager::proxy( const QNetworkRequest &request ) const
{
    QNetworkProxy proxy;
    
#ifdef WIN32
	IeSettings s;
    if (s.fAutoDetect) 
    {
        if (!m_pac) {
            m_pac = new Pac;
            m_monitor = new WsConnectionMonitor( this );
            connect( m_monitor, SIGNAL(connectivityChanged( bool )), SLOT(onConnectivityChanged( bool )) );
        }
		proxy = m_pac->resolve( request, s.lpszAutoConfigUrl );
	} 
    else if (s.lpszProxy)
    {
		// manual proxy
		QUrl url( QString::fromUtf16(s.lpszProxy) );
		proxy.setHostName( url.host() );
		proxy.setPort( url.port() );
	}
#endif
#ifdef __APPLE__
    static ProxyDict dict;
    proxy.setHostName( dict.host );
    proxy.setPort( dict.port );
#endif

    return proxy;
}


QNetworkReply*
WsAccessManager::createRequest( Operation op, const QNetworkRequest& request_, QIODevice* outgoingData )
{
    QNetworkRequest request = request_;

    request.setRawHeader( "User-Agent", Ws::UserAgent );
    QNetworkProxy proxy = this->proxy( request );
    if (proxy.type() != QNetworkProxy::NoProxy)
        QNetworkAccessManager::setProxy( proxy );
	return QNetworkAccessManager::createRequest( op, request, outgoingData );
}


void
WsAccessManager::onConnectivityChanged( bool up )
{
    Q_UNUSED( up );
    
#ifdef WIN32
    if (up && m_pac) m_pac->resetFailureState();
#endif
}

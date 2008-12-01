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


struct WsAccessManagerInit
{
    // We do this upfront because then our Firehose QTcpSocket will have a proxy 
    // set by default. As well as any plain QNetworkAcessManager stuff, and the
    // scrobbler
    // In theory we should do this every request in case the configuration 
    // changes but that is fairly unlikely use case, init? Maybe we should 
    // anyway..

    WsAccessManagerInit()
    {
    #ifdef WIN32
        IeSettings s;
        // if it's autodetect, we determine the proxy everytime in proxy()
        if (!s.fAutoDetect && s.lpszProxy)
        {
            QUrl url( QString::fromUtf16(s.lpszProxy) );
            QNetworkProxy proxy( QNetworkProxy::HttpProxy );
            proxy.setHostName( url.host() );
            proxy.setPort( url.port() );
            QNetworkProxy::setApplicationProxy( proxy );
        }
    #endif
    #ifdef __APPLE__
        ProxyDict dict;
        if (dict.isProxyEnabled())
        {
            QNetworkProxy proxy( QNetworkProxy::HttpProxy );
            proxy.setHostName( dict.host );
            proxy.setPort( dict.port );

            QNetworkProxy::setApplicationProxy( proxy );
        }
    #endif
    }
};
static WsAccessManagerInit init;    


WsAccessManager::WsAccessManager( QObject* parent )
               : QNetworkAccessManager( parent )
            #ifdef WIN32
               , m_pac( 0 )
               , m_monitor( 0 )
            #endif
{
    // can't be done in above init, as applicationName() won't be set
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
WsAccessManager::proxy( const QNetworkRequest& request )
{   
    Q_UNUSED( request );
    
#ifdef WIN32
	IeSettings s;
    if (s.fAutoDetect) 
    {
        if (!m_pac) {
            m_pac = new Pac;
            m_monitor = new WsConnectionMonitor( this );
            connect( m_monitor, SIGNAL(connectivityChanged( bool )), SLOT(onConnectivityChanged( bool )) );
        }
		return m_pac->resolve( request, s.lpszAutoConfigUrl );
	} 
#endif
    
    return QNetworkProxy::applicationProxy();
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
    if (up && m_pac) m_pac->resetFailedState();
#endif
}

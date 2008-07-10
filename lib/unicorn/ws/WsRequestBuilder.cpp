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

#include "WsRequestBuilder.h"
#include "WsReply.h"
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QEventLoop>

QNetworkAccessManager* WsRequestBuilder::nam = 0;


WsRequestBuilder::WsRequestBuilder( const QString& method )
{
    if (!nam) nam = new QNetworkAccessManager( qApp );
    
    params.add( "method", method );
}


WsReply*
WsRequestBuilder::start()
{
    QUrl url( "http://ws.audioscrobbler.com/2.0/" );
    url.setQueryItems( params );

    QNetworkRequest request( url );
    request.setRawHeader( "User-Agent", userAgent() );
    QNetworkReply* reply = 0;

    switch (request_method)
    {
        case GET:  
            reply = nam->get( request ); 
            break;

        case POST: 
            reply = nam->post( request, url.encodedQuery() ); 
            break;

        default:
            Q_ASSERT( !"Unknown RequestType" );
            break;
    }

    return new WsReply( reply );
}


QByteArray //static
WsRequestBuilder::userAgent()
{
    //FIXME since we aren't all the client
    // NEVER CHANGE THE FOLLOWING STRING! you can append stuff, but that's it
    QByteArray agent = "Last.fm Client";
#ifdef WIN32
    agent += " (Windows)";
#elif defined (Q_WS_MAC)
    agent += " (OS X)";
#elif defined (Q_WS_X11)
    agent += " (X11)";
#endif
    return agent;
}

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
#ifndef Q_WS_MAC
    QUrl url( !qApp->arguments().contains( "--debug")
            ? "http://ws.audioscrobbler.com/2.0/"
            : "http://ws.staging.audioscrobbler.com/2.0/" );
#else
    QUrl url( "http://ws.audioscrobbler.com/2.0/" );    
#endif

	//Only GET requests should include query parameters
	if( request_method == GET )
		url.setQueryItems( params );
	
    QNetworkRequest request( url );
    request.setRawHeader( "User-Agent", userAgent() );

    switch (request_method)
    {
        case GET:  return new WsReply( nam->get( request ) );
        case POST: 
		{
			//Build encoded query for use in the POST Content
			QByteArray query;
			QList<QPair<QString, QString> > paramList = params;
			for( int i = 0; i < paramList.count(); i++ )
			{
				query += QUrl::toPercentEncoding( paramList[ i ].first, "!$&'()*+,;=:@/?" )
					  + "="
					  + QUrl::toPercentEncoding( paramList[ i ].second, "!$&'()*+,;=:@/?" )
					  + "&";
			}
			
			return new WsReply( nam->post( request, query ) );
		}
    }
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

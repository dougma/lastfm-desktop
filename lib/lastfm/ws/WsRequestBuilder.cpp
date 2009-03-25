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

#include "WsRequestBuilder.h"
#include "WsKeys.h"
#include "WsReply.h"
#include "WsRequestParameters.h"
#include "WsAccessManager.h"
#include <QCoreApplication>
#include <QEventLoop>
#include <QMutex>
#include <QMutexLocker>
#include <QThread>


QThreadStorage<WsAccessManager*> WsRequestBuilder::nam_store;


WsRequestBuilder::WsRequestBuilder( const QString& method )
{
    static QMutex lock;
    QMutexLocker locker( &lock );

    params["method"] = method;
}


WsRequestBuilder&
WsRequestBuilder::add( const QString& key, const QString& value )
{
    if (!key.isEmpty())
        params[key] = value;
    return *this; 
}


WsReply*
WsRequestBuilder::start( RequestMethod method )
{
    WsRequestParameters wsparams;
    
    QMapIterator<QString, QString> i( params );
    while (i.hasNext()) {
        i.next();
        wsparams.add( i.key(), i.value() );
    }
    
    QUrl url( !qApp->arguments().contains( "--debug")
            ? "http://" LASTFM_WS_HOSTNAME "/2.0/"
            : "http://ws.staging.audioscrobbler.com/2.0/" );

    typedef QPair<QString, QString> Pair; // don't break foreach macro
    QList<Pair> params( wsparams );

    switch (method)
    {
        case Get:
        {
            // Qt setQueryItems doesn't encode a bunch of stuff, so we do it manually
            foreach (Pair pair, params)
            {
                QByteArray const key = QUrl::toPercentEncoding( pair.first );
                QByteArray const value = QUrl::toPercentEncoding( pair.second );
                url.addEncodedQueryItem( key, value );
            }
            return new WsReply( nam()->get( QNetworkRequest( url ) ) );
        }

        case Post:
		{
			QByteArray query;
			foreach (Pair param, params)
			{
				query += QUrl::toPercentEncoding( param.first )
					  + "="
					  + QUrl::toPercentEncoding( param.second )
					  + "&";
			}
			return new WsReply( nam()->post( QNetworkRequest( url ), query ) );
		}
    }
	return 0;
}


WsAccessManager* //static private
WsRequestBuilder::nam()
{
    if (!nam_store.hasLocalData())
        // WsAccessManager will be unparented, but it
        // does gets cleaned up when this thread ends
        nam_store.setLocalData( new WsAccessManager );

    return nam_store.localData();
}

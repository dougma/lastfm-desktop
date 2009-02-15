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

#ifndef LASTFM_WS_REQUEST_MANAGER_H
#define LASTFM_WS_REQUEST_MANAGER_H

#include <lastfm/WsReply> //for your convenience <3 x
#include <QMap>
#include <QThreadStorage>


/** A convenience class to create a WsReply for Last.fm webservices.
  * We add the session key, api signature and session key to every request */
class LASTFM_WS_DLLEXPORT WsRequestBuilder
{
    static QThreadStorage<class WsAccessManager*> nam; // one per thread
    QMap<QString, QString> params;

    enum RequestMethod { Get, Post };
    /** starts the request, connect to finished() to get the results */
    WsReply* start( RequestMethod );

public:
    WsRequestBuilder( const QString& methodName );

    WsReply* get() { return start( Get ); }
    WsReply* post() { return start( Post ); }

    /** add a parameter to the request, if @p key isEmpty() we silently ignore it */
    WsRequestBuilder& add( const QString& key, const QString& value );
    /** for convenience */
    WsRequestBuilder& add( const QString& key, int const value ) { return add( key, QString::number( value ) ); }
    /** for convenience */
    WsRequestBuilder& addIfNotEmpty( const QString& key, const QString& s ) { if (s.size()) add( key, s ); return *this; }
};

#endif

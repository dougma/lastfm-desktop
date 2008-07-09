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

#ifndef WS_REQUEST_MANAGER_H
#define WS_REQUEST_MANAGER_H

#include "WsRequestParameters.h"
#include "lib/DllExportMacro.h"

class WsReply;

   
class UNICORN_DLLEXPORT WsRequestBuilder
{
    enum RequestMethod
    {
        GET,
        POST
    };

    static class QNetworkAccessManager* nam;

    RequestMethod request_method;
    WsRequestParameters params;

    static QByteArray userAgent();

public:
    WsRequestBuilder( const QString& methodName );
    
    WsRequestBuilder& get() { request_method = GET; return *this; }
    WsRequestBuilder& post() { request_method = POST; return *this; }

    /** add a parameter to the request */
    WsRequestBuilder& add( const QString& key, const QString& value ) { params.add( key, value ); return *this; }

    WsReply* synchronously();
    WsReply* asynchronously();
};

#endif

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

#include "Album.h"
#include "Artist.h"
#include "User.h"
#include "lib/ws/WsRequestBuilder.h"
#include <QEventLoop>


WsReply*
Album::getInfo() const
{
    return WsRequestBuilder( "album.getInfo" )
            .add( "artist", m_artist )
            .add( "album", m_title )
            .get();
}


QByteArray
Album::image()
{
    WsReply* reply = getInfo();
    reply->finish();

    try
    {
        QUrl url = reply->lfm()["album"]["image size=large"].text();

        QNetworkAccessManager manager;
        QNetworkReply* get = manager.get( QNetworkRequest( url ) );
        QEventLoop loop;
        QObject::connect( get, SIGNAL(finished()), &loop, SLOT(quit()) );
        loop.exec();
        QByteArray bytes = get->readAll();
        delete get;
        return bytes;
    }
    catch (EasyDomElement::Exception& e)
    {
        qWarning() << e;
    }

    return "";
}


WsReply*
Album::share( const User& recipient, const QString& message )
{
    return WsRequestBuilder( "album.share" )
        .add( "recipient", recipient )
        .add( "artist", m_artist )
        .add( "album", m_title )
        .addIfNotEmpty( "message", message )
        .post();
}

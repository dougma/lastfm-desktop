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
#include "ws/WsRequestBuilder.h"
#include "ws/WsReply.h"
#include <QEventLoop>


WsReply*
Album::getInfo() const
{
    return WsRequestBuilder( "Album.getInfo" )
            .add( "artist", m_artist )
            .add( "album", m_title )
            .get();
}


QPixmap
Album::image()
{
    WsReply* reply = getInfo();
    reply->finish();

    try
    {
        QUrl url = reply->lfm()["album"]["image size=small"].text();

        QNetworkAccessManager manager;
        QNetworkReply* get = manager.get( QNetworkRequest( url ) );
        QEventLoop loop;
        QObject::connect( get, SIGNAL(finished()), &loop, SLOT(quit()) );
        loop.exec();
        QByteArray bytes = get->readAll();
        delete get;
        QPixmap p;
        p.loadFromData( bytes );
        return p;
    }
    catch (EasyDomElement::Exception& e)
    {
        qWarning() << e;
    }

    return QPixmap();
}
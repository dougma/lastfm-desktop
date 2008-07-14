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

#include "Radio.h"
#include "lib/unicorn/UnicornSettings.h"
#include <QBuffer>
#include <QDebug>
#include "lib/unicorn/ws/WsRequestBuilder.h"
#include "lib/unicorn/ws/WsReply.h"
#include <QtNetwork/QHttp> //TODO use our override
#include <QtXml>

//TODO discovery mode
//TODO skips left
//TODO multiple locations for the same track
//TODO set rtp flag in getPlaylist (whether user is scrobbling this radio session or not)


Radio::Radio( const QString& username, const QString& password )
     : m_username( username ),
       m_password( password ),
       m_done( false )
{}


void
Radio::tuneIn( const QString& url )
{
    m_station_url = url;
    start();   
}


void
Radio::fetchNextPlaylist()
{
    m_waitCondition.wakeAll();
}


void
Radio::run()
{
    while (!m_done)
    {
        try
        {
            m_host = "ws.audioscrobbler.com";
            m_base_path = "/radio";

            adjust();
            for (;;)
            {
                QList<Track> tracks = fetchPlaylist();

                foreach (Track t, tracks)
                {
                    qDebug() << t.artist << t.title << t.location;
                }
                
                emit this->tracks( tracks );

                QMutex mutex;
                mutex.lock();
                m_waitCondition.wait( &mutex );
                mutex.unlock();
                qDebug() << "woke up!";
            }
        }
        catch (int)
        {}
    }
}


struct Map : QMap<QByteArray, QString>
{
    Map( const QByteArray& in )
    {
        foreach (QByteArray line, in.split( '\n' ))
        {
            int const n = line.indexOf( '=' );
            QByteArray key = line.left( n );
            QString value = QString::fromUtf8( line.mid( n + 1 ) );

            operator[]( key ) = value;
            qDebug() << key << value;
        }
    }
};


void
Radio::adjust()
{
    WsRequestBuilder wsBuilder( "radio.tune" );
    wsBuilder.add( "station", m_station_url );
    WsReply* reply = wsBuilder.post();

    reply->finish();
}


QList<Radio::Track>
Radio::fetchPlaylist()
{
    WsRequestBuilder wsBuilder( "radio.getPlaylist" );

    WsReply* reply = wsBuilder.get();

    reply->finish();

    EasyDomElement xml = reply->lfm();

    QList<Track> tracks;

    QList<EasyDomElement> trackElements =  xml["playlist"][ "trackList" ].children( "track" );
    for (int x = 0; x < trackElements.count(); ++x)
    {
        EasyDomElement trackElement = trackElements.at( x );

        Track t;
        t.authcode = trackElement[ "extension" ][ "trackauth" ].text();
        t.title = trackElement[ "title" ].text();
        t.artist = trackElement[ "creator" ].text();
        t.album = trackElement[ "album" ].text();
        t.duration = trackElement[ "duration" ].text().toInt();
        t.location = trackElement[ "location" ].text();
        
        tracks += t;
    }

    return tracks;
}

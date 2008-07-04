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
#include <QtNetwork/QHttp> //TODO use our override
#include <QtXml>

//TODO discovery mode
//TODO skips left
//TODO multiple locations for the same track


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

            handshake();
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


QByteArray
Radio::get( QString path )
{
    //TODO error handling

    QHttp http( m_host );

    path.prepend( m_base_path );
    qDebug() << "Radio::GET" << m_host + path;

    http.get( path );

    QEventLoop loop;
    connect( &http, SIGNAL(requestFinished( int, bool )), &loop, SLOT(quit()) );
    loop.exec();

    return http.readAll();
}


void
Radio::handshake()
{
    QString path = "/handshake.php"
                   "?version=2.0" //FIXME
                   "&platform="
                   "&platfromversion="
                   "&username=" + m_username +
                   "&passwordmd5=" + m_password +
                   "&language=" + Unicorn::Settings().language();

    Map map = get( path );

    m_session = map["session"];
    m_host = map["base_url"];

    qDebug() << map;
}


void
Radio::adjust()
{
    QString path = "/adjust.php?session=" + m_session + "&url=" + m_station_url + "&lang=" + Unicorn::Settings().language();

    get( path );
}


QList<Radio::Track>
Radio::fetchPlaylist()
{
    QString path = "/xspf.php?sk=" + m_session + "&discovery=0&desktop=2.0"; //FIXME
    QByteArray data = get( path );

    QDomDocument xml;
    xml.setContent( data ); //presumably figures out encoding as XML should specify in xml header?

    QList<Track> tracks;
    QDomNodeList nodes = xml.documentElement().firstChildElement( "trackList" ).elementsByTagName( "track" );
    for (int x = 0; x < nodes.count(); ++x)
    {
        QDomNode n = nodes.at( x );

        #define h( x ) n.firstChildElement( x ).text()
        Track t;
        t.authcode = h("lastfm:authcode");
        t.title = h("title");
        t.artist = h("creator");
        t.album = h("album");
        t.duration = h("duration").toInt();
        t.sponsor = h("lastfm:sponsored");
        t.location = h("location");
        #undef h
        
        tracks += t;
    }

    return tracks;
}

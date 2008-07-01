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

#include "PrivateMockScrobblerHttp.h"

PrivateMockScrobblerHttp* PrivateMockScrobblerHttp::s_instance = 0;

PrivateMockScrobblerHttp::PrivateMockScrobblerHttp()
{
    s_instance = this;
}

PrivateMockScrobblerHttp::~PrivateMockScrobblerHttp()
{
    s_instance = 0;
}

QMap<QString, QString>
PrivateMockScrobblerHttp::extractArgs( const QString& line )
{
    QMap<QString, QString> map;

    foreach (QString pair, line.split( '&', QString::SkipEmptyParts ))
    {
        QStringList parts = pair.split( '=' );

        //if (parts.count() != 2 || parts[0].length() > 1) 
        //    throw Exception( "Invalid pair: " + pair );
                
        QString id = parts[0];

        //if (map.contains( id ))
        //    throw Exception( "Field identifier occurred twice in request: " + QString(id) );

        map[id] = parts[1].trimmed();
    }

    return map;
}

void
PrivateMockScrobblerHttp::handshake ( QString data )
{
    data.remove( 0, 2 );
    QMap<QString, QString> args = extractArgs( data );
    
    m_handshake = false;
    if ( args["hs"] == "true" )
        m_handshake = true;
    m_protocolver = args["p"];
    m_clientname = args["c"];
    m_clientver = args["v"];
    m_username = args["u"];
    m_timestamp = args["t"].toInt();
    m_authtoken = args["a"];
}

void
PrivateMockScrobblerHttp::nowPlaying ( QString data )
{
    data.remove( 0, 1 );
    QMap<QString, QString> args = extractArgs( data );

    qDebug() << "PrivateMockScrobblerHttp::nowPlaying " << data;

    // We should use toLatin1 since percent encoding only should contain ascii characters
    m_sessionid = QUrl::fromPercentEncoding( args["s"].toAscii() ); 
    m_artist = QUrl::fromPercentEncoding( args["a"].toAscii() );
    m_track = QUrl::fromPercentEncoding( args["t"].toAscii() );
    m_album = QUrl::fromPercentEncoding( args["b"].toAscii() );
    m_duration = args["l"].toInt();
    m_musicbrainz = args["m"];
}

void
PrivateMockScrobblerHttp::submitTrack ( QString data )
{
    data.remove( 0, 1 );
    QMap<QString, QString> args = extractArgs( data );

    qDebug() << "PrivateMockScrobblerHttp::submitTrack " << data;

    // We should use toLatin1 since percent encoding only should contain ascii characters
    m_sessionid = QUrl::fromPercentEncoding( args["s[0]"].toAscii() ); 
    m_artist = QUrl::fromPercentEncoding( args["a[0]"].toAscii() );
    m_track = QUrl::fromPercentEncoding( args["t[0]"].toAscii() );
    m_album = QUrl::fromPercentEncoding( args["b[0]"].toAscii() );
    m_duration = args["l[0]"].toInt();
    m_musicbrainz = args["m[0]"];
}




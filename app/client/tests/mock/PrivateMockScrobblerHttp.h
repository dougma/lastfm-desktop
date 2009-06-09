/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "ScrobblerHttp.h"

#include <QDebug>
#include <QTimer>
#include <QMap>

class PrivateMockScrobblerHttp : public QObject
{
public:
    PrivateMockScrobblerHttp();
    ~PrivateMockScrobblerHttp();

    static PrivateMockScrobblerHttp* s_instance;

    void handshake ( QString );
    void nowPlaying( QString );
    void submitTrack( QString );
    
    QByteArray readResponse() { return m_nextResponse; }
    void setNextResponse( QByteArray r ) { m_nextResponse = r; }
    
    bool handshake() { return m_handshake; }
    QString protocolVersion() { return m_protocolver; }
    QString clientName() { return m_clientname; }
    QString clientVersion() { return m_clientver; }
    QString username() { return m_username; }
    QString authToken() { return m_authtoken; }
    int timestamp() { return m_timestamp; }
    
    QString sessionId() { return m_sessionid; }
    
    QString artist() { return m_artist; }
    QString track() { return m_track; }
    QString album() { return m_album; }
    QString musicbrainz() { return m_musicbrainz; }
    int duration() { return m_duration; }
    int trackNumber() { return m_tracknr; }
    
protected:
    QMap<QString, QString> extractArgs( const QString& line );
    
    bool m_handshake;
    int m_timestamp;
    QString m_protocolver, m_clientname, m_clientver, m_username, m_authtoken, m_sessionid;
    
    QString m_artist, m_track, m_album, m_musicbrainz;
    int m_duration, m_tracknr;
    
    QByteArray m_nextResponse;
};


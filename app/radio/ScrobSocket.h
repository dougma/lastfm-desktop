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
#ifndef SCROB_SOCKET_H
#define SCROB_SOCKET_H

#include <lastfm/global.h>
#include <lastfm/Track>
#include <QLocalSocket>
#include <QQueue>

/** @author Christian Muehlhaeuser <chris@last.fm>
  * @contributor Erik Jaelevik <erik@last.fm>
  * @rewrite Max Howell <max@last.fm>
  */
class ScrobSocket : public QLocalSocket
{
    Q_OBJECT

public:
    ScrobSocket( const QString& clientId, QObject* parent = 0);
    ~ScrobSocket();

public slots:
    void start( const Track& );
    void pause();
    void resume();
    void stop();

private slots:
    void transmit( const QString& data );
    void onError( QLocalSocket::LocalSocketError );
    void onReadyRead();
    void onConnected();
    void onDisconnected();

private:
    void doConnect();

    Track m_track;
    QQueue<QString> m_msgQueue;
    bool m_bInConnect;
    QString m_clientId;
};

#endif

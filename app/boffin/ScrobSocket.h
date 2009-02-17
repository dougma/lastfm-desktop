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

#ifndef SCROB_SOCKET_H
#define SCROB_SOCKET_H

#include <lastfm/global.h>
#include <lastfm/Track>
#include <QTcpSocket>

/** @author Christian Muehlhaeuser <chris@last.fm>
  * @contributor Erik Jaelevik <erik@last.fm>
  * @rewrite Max Howell <max@last.fm>
  */
class ScrobSocket : public QTcpSocket
{
    Q_OBJECT

public:
    ScrobSocket( QObject* parent );
    ~ScrobSocket();

public slots:
    void start( const lastfm::Track& );
    void pause();
    void resume();
    void stop();

private slots:
    void transmit( const QString& data );
    void onError( QAbstractSocket::SocketError );

private:    
    Track m_track;
};

#endif

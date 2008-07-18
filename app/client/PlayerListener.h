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

#ifndef PLAYER_LISTENER_H
#define PLAYER_LISTENER_H

#include "common/HideStupidWarnings.h"
#include <QTcpServer>
class Track;


/** @author Erik Jaelevik, <erik@last.fm>
  * @contributor Christian Muehlhaeuser <chris@last.fm>
  * @rewrite Max Howell <max@last.fm> -- to Qt4
  */
class PlayerListener : public QTcpServer
{
    Q_OBJECT

public:
    struct SocketFailure : private QString
    {
        SocketFailure( const QString& what ) : QString( what )
        {}
        
        QString what() const { return *this; }
    };

    PlayerListener( QObject* parent = 0 ) throw( SocketFailure );

    uint port() const { return 33367; }
    
signals:
    void trackStarted( const Track& );
    void playbackEnded( const QString& playerId );
    void playbackPaused( const QString& playerId );
    void playbackResumed( const QString& playerId );
    void bootstrapCompleted( const QString& playerId, const QString& username );
    void playerInit( const QString& playerId );
    void playerTerm( const QString& playerId );

private slots:
    void onNewConnection();
    void onDisconnected( QAbstractSocket::SocketState );
    void onDataReady();

private:
    QMap< QTcpSocket*, QString > m_socketMap;
};

#endif

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

// ms admits its lousy compiler doesn't care about throw declarations
#pragma warning( disable : 4290 ) 

#include <QTcpServer>
class TrackInfo;
class PlayerConnection;


 /* @author Erik Jaelevik, <erik@last.fm>
  * @contributor Christian Muehlhaeuser <chris@last.fm>
  * @rewrite Max Howell <max@last.fm> -- to Qt4
  */

class PlayerListener : public QTcpServer
{
    Q_OBJECT

public:
    class SocketFailure
    {};

    PlayerListener( QObject* parent = 0 ) throw( SocketFailure );

signals:
    void trackStarted( const TrackInfo& );
    void playbackEnded( const QString& playerId );
    void playbackPaused( const QString& playerId );
    void playbackResumed( const QString& playerId );
    void bootstrapCompleted( const QString& playerId, const QString& username );

private slots:
    void onNewConnection();
};

#endif

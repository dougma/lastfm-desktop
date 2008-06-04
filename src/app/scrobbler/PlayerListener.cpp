/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *    This program is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include "PlayerListener.h"
#include "PlayerConnection.h"


PlayerListener::PlayerListener( QObject* parent )
              : QTcpSocket( parent )
{
    connect( this, SIGNAL(newConnection()), SLOT(onNewConnection()) );

    if (!listen( QHostAddress::LocalHost, 33367 ))
        throw ListenFailure();
}


void
PlayerListener::onNewConnection()
{
    while (hasPendingConnection())
    {
        PlayerConnection* c = new PlayerConnection( nextPendingConnection() );
        m_connections.push( c );

        connect( c, SIGNAL(trackStarted( TrackInfo )), SIGNAL(trackStarted( TrackInfo )) );
        connect( c, SIGNAL(playbackEnded()), SLOT(onPlaybackEnded()) );
        connect( c, SIGNAL(playbackPaused()), SLOT(onPlaybackPaused()) );
        connect( c, SIGNAL(playbackResumed()), SLOT(onPlaybackResumed()) );
        connect( c, SIGNAL(bootstrapCompleted( QString )), SLOT(onBootstrapCompleted( QString )) );
    }
}


static inline QString id( QObject*o )
{
    return static_cast<PlayerConnection*>(o)->playerId();
}


void
PlayerListener::onPlaybackEnded()
{
    emit playbackStopped( id(sender()) );
}


void
PlayerListener::onPlaybackPaused()
{
    emit playbackPaused( id(sender()) );
}


void
PlayerListener::onPlaybackResumed()
{
    emit playbackResumed( id(sender()) );
}


void
PlayerListener::onBootstrapComplete( const QString& username )
{
    emit bootstrapComplete( id(sender()), username );
}

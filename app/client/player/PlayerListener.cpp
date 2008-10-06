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
#include "PlayerCommandParser.h"
#include "PlayerConnection.h"
#include <QTcpSocket>


PlayerListener::PlayerListener( QObject* parent ) throw( PlayerListener::SocketFailure )
              : QTcpServer( parent )
{
    connect( this, SIGNAL(newConnection()), SLOT(onNewConnection()) );

    //TODO port stepping?
    if (!listen( QHostAddress::LocalHost, port() ))
        throw SocketFailure( errorString() );
}


void
PlayerListener::onNewConnection()
{
    while (hasPendingConnections())
    {
        QTcpSocket* socket = nextPendingConnection();
        connect( socket, SIGNAL(readyRead()), SLOT(onDataReady()) );
        connect( socket, SIGNAL(disconnected()), SLOT(onDisconnection()) );
    }
}


void
PlayerListener::onDisconnection()
{
    QTcpSocket* socket = (QTcpSocket*)sender();
 
    if (!m_connections.contains( socket ))
        // already handled
        return;
    
    m_connections[socket].clear();
    m_connections[socket].command = PlayerCommandParser::Term;
    emit playerCommand( m_connections[socket] );

    term( socket );
}


void
PlayerListener::onDataReady()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    while (socket->canReadLine())
    {
		try
        {
            PlayerCommandParser parser( QString::fromUtf8( socket->readLine() ) );
            PlayerConnection& connection = m_connections[socket];
            connection.command = parser.command();
            
            switch ((int)connection.command)
            {
                case PlayerCommandParser::Pause:
                case PlayerCommandParser::Resume:
                    if (connection.track.isNull())
                    {
                        qWarning() << "Cannot pause or resume null track";
                        connection.command = PlayerCommandParser::Stop;
                    }
            }
            
            switch (connection.command)
            {
                case PlayerCommandParser::Init:
                    connection.id = parser.playerId();
                    connection.name = connection.determineName();
                    connection.clear();
                    break;
                    
                case PlayerCommandParser::Start:
                    connection.state = Playing;
                    connection.track = parser.track();
                    break;
                    
                case PlayerCommandParser::Pause:
                    connection.state = Paused;
                    break;
                    
                case PlayerCommandParser::Term:
                    term( socket );
                    // fall through
                case PlayerCommandParser::Stop:
                    connection.clear();
                    break;
                    
                case PlayerCommandParser::Resume:
                    connection.state = Playing;
                    break;
                    
                case PlayerCommandParser::Bootstrap:
                    emit bootstrapCompleted( parser.playerId() );
                    socket->write( "OK\n" );
                    break;
            }
            
            emit playerCommand( connection );
            socket->write( "OK\n" );
        }
        catch (PlayerCommandParser::Exception& e)
        {
            qWarning() << e;
            QString s = "ERROR: " + e.what() + "\n";
            socket->write( s.toUtf8() );
        }
    }
}


void
PlayerListener::term( QTcpSocket* socket )
{
    socket->deleteLater();
    m_connections.remove( socket );
}

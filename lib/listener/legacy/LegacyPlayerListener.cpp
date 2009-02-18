/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
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

#include "LegacyPlayerListener.h"
#include "../PlayerCommandParser.h"
#include "../PlayerConnection.h"
#include <QTcpSocket>


LegacyPlayerListener::LegacyPlayerListener( QObject* parent )
                    : QTcpServer( parent )
{
    connect( this, SIGNAL(newConnection()), SLOT(onNewConnection()) );
    if (!listen( QHostAddress::LocalHost, port() ))
        qWarning() << "Couldn't start legacy player listener";
}


void
LegacyPlayerListener::onNewConnection()
{
    while (hasPendingConnections())
    {
        QTcpSocket* socket = nextPendingConnection();
        connect( socket, SIGNAL(readyRead()), SLOT(onDataReady()) );
    }
}


void
LegacyPlayerListener::onDataReady()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    connect( socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()) );    
    
    while (socket->canReadLine())
    {
        QString line = QString::fromUtf8( socket->readLine() );
        
		try
        {
            PlayerCommandParser parser( line );

            QString const id = parser.playerId();
            PlayerConnection* connection = 0;

            if (!m_connections.contains( id )) {
                connection = m_connections[id] = new PlayerConnection( parser.playerId(), parser.playerName() );
                emit newConnection( connection );
            }
            else
                connection = m_connections[id];
            
            switch (parser.command())
            {
                case CommandBootstrap:
                    qWarning() << "We no longer support Bootstrapping with the LegacyPlayerListener";
                    break;
                    
                case CommandTerm:
                    m_connections.remove( id );
                    // FALL THROUGH
                    
                default:
                    connection->handleCommand( parser.command(), parser.track() );
                    break;
            }
            
            socket->write( "OK\n" );
        }
        catch (CoreException& e)
        {
            qWarning() << line << e;
            QString s = "ERROR: " + e.what() + "\n";
            socket->write( s.toUtf8() );
        }
    }
    
    socket->close();
}

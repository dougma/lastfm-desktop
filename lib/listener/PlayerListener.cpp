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
#include <QLocalSocket>

#ifdef WIN32
    #include "common/c++/win/scrobSubPipeName.cpp"
#endif


PlayerListener::PlayerListener( QObject* parent ) throw( PlayerListener::SocketFailure )
              : QLocalServer( parent )
{
    connect( this, SIGNAL(newConnection()), SLOT(onNewConnection()) );

#ifdef WIN32
    std::string s;
    DWORD r = scrobSubPipeName( &s );
    if (r != 0) throw SocketFailure( QString::fromStdString( formatWin32Error( r ) ) );
    QString const name = QString::fromStdString( s );
#else
    QString const name = "lastfm_scrobsub";
#endif
    
    if (!listen( name ))
        throw SocketFailure( errorString() );
}


void
PlayerListener::onNewConnection()
{
    while (hasPendingConnections())
    {
        QObject* o = nextPendingConnection();
        connect( o, SIGNAL(readyRead()), SLOT(onDataReady()) );
        connect( o, SIGNAL(disconnected()), o, SLOT(deleteLater()) );
    }
}

void
PlayerListener::onDataReady()
{
    QLocalSocket* socket = qobject_cast<QLocalSocket*>(sender());
    if (!socket) return;

    while (socket->canReadLine())
    {
        QString const line = QString::fromUtf8( socket->readLine() );
        
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
                    emit bootstrapCompleted( parser.playerId() );
                    break;
                    
                case CommandTerm:
                    delete connection;
                    m_connections.remove( parser.playerId() );
                    break;
                    
                default:
                    connection->handleCommand( parser.command(), parser.track() );
                    break;
            }
            
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

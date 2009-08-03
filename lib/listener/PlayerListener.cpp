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
#include "PlayerListener.h"
#include "PlayerCommandParser.h"
#include "PlayerConnection.h"
#include <QLocalSocket>
#include <QDir>
#include <QFile>

#ifdef WIN32
    #include "common/c++/win/scrobSubPipeName.cpp"
#endif


PlayerListener::PlayerListener( QObject* parent ) throw( std::runtime_error )
              : QLocalServer( parent )
{
    connect( this, SIGNAL(newConnection()), SLOT(onNewConnection()) );

    // Create a user-unique name to listen on.
    // User-unique so that different logged-on users 
    // can run their own scrobbler instances.

#ifdef WIN32
    std::string s;
    DWORD r = scrobSubPipeName( &s );
    if (r != 0) throw std::runtime_error( formatWin32Error( r ) );
    QString const name = QString::fromStdString( s );
#else
    QString const name = "lastfm_scrobsub";

    // on windows we use named pipes which auto-delete
    // *nix platforms need more help:

    // todo: need to make this user-unique
    if( QFile::exists( QDir::tempPath() + "/" + name ))
        QFile::remove( QDir::tempPath() + "/" + name );
#endif
    
    if (!listen( name ))
        throw std::runtime_error( errorString().toStdString() );
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
        catch (std::invalid_argument& e)
        {
            qWarning() << e.what();
            QString s = "ERROR: " + QString::fromStdString(e.what()) + "\n";
            socket->write( s.toUtf8() );
        }
    }
}

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
#include "lib/unicorn/Logger.h"
#include <QTcpSocket>


PlayerListener::PlayerListener( QObject* parent )
              : QTcpServer( parent )
{
    connect( this, SIGNAL(newConnection()), SLOT(onNewConnection()) );

    //TODO stepping
    if (!listen( QHostAddress::LocalHost, 33367 ))
        throw SocketFailure();
}


void
PlayerListener::onNewConnection()
{
    Q_DEBUG_BLOCK;

    while (hasPendingConnections())
    {
        QTcpSocket* socket = nextPendingConnection();

        socket->waitForReadyRead(); //FIXME, blocks

        while (socket->canReadLine())
        {
            try
            {
                PlayerCommandParser parser( socket->readLine() );

                switch (parser.command())
                {
                    case PlayerCommandParser::Start:
                        emit trackStarted( parser.track() );
                        break;
                    case PlayerCommandParser::Stop:
                        emit playbackEnded( parser.playerId() );
                        break;
                    case PlayerCommandParser::Pause:
                        emit playbackPaused( parser.playerId() );
                        break;
                    case PlayerCommandParser::Resume:
                        emit playbackResumed( parser.playerId() );
                        break;
                    case PlayerCommandParser::Bootstrap:
                        emit bootstrapCompleted( parser.playerId(), parser.username() );
                        break;
                }

                socket->write( "OK\n" );
            }
            catch (PlayerCommandParser::Exception& e)
            {
                LOGL( 2, e );
                QString s = "ERROR: " + e + "\n";
                socket->write( s.toUtf8() );
            }
        }

        socket->waitForBytesWritten(); //FIXME, blocks
        delete socket;
    }
}

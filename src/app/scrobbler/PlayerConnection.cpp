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

#include "PlayerConnection.h"
#include "PlayerCommandParser.h"

#include "lib/unicorn/Logger.h"

#include <QTcpSocket>


PlayerConnection( QTcpSocket* socket ) : m_socket( socket )
{
    if (m_socket->canReadLine())
        parse();

    connect( socket, SIGNAL(destroyed()), SLOT(deleteLater()) );
    connect( socket, signal(readyRead()), SLOT(parse()) );
}


void
PlayerConnection::parseLine()
{
    while (m_socket->canReadLine())
    {
        QString const line =  m_socket->readLine();

        try
        {
            PlayerCommandParser parser( line );

            switch (parser.command())
            {
                case PlayerCommandParser::Start:
                    emit trackChanged( parser.track() );
                    break;
                case PlayerCommandParser::Stop:
                    emit playbackStopped();
                    break;
                case PlayerCommandParser::Pause:
                    emit playbackPaused();
                    break;
                case PlayerCommandParser::Resume:
                    emit playbackResumed();
                    break;
                case PlayerCommandParser:::BootStrap:
                    emit bootstrapComplete( parser.username() );
                    break;
            }
        }
        catch (PlayerCommandParser::Exception& e)
        {
            LOGL( 2, e );
        }
    }
}

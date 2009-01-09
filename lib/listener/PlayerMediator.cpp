/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd                                       *
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

#include "PlayerMediator.h"


PlayerMediator::PlayerMediator( QObject* parent )
              : QObject( parent )
              , m_active( 0 )
{}


void
PlayerMediator::follow( PlayerConnection* connection )
{
    if (m_connections.contains( connection )) { qWarning() << "Already following:" << connection; return; }
    
    m_connections += connection;

    connect( connection, SIGNAL(trackStarted( Track )), SLOT(onActivity()) );
    connect( connection, SIGNAL(resumed()), SLOT(onActivity()) );
    connect( connection, SIGNAL(stopped()), SLOT(onActivity()) );
    connect( connection, SIGNAL(destroyed()), SLOT(onDestroyed()) );

    assess( connection );
}


void
PlayerMediator::onActivity()
{
    PlayerConnection* connection = (PlayerConnection*)sender();

    if (m_active == connection)
    {
        if (connection->state() == Stopped)
        {
            foreach (PlayerConnection* connection, m_connections)
                if (connection != m_active && assess( connection ))
                        return;
        }
    }
    else
        assess( connection );
}


bool
PlayerMediator::assess( PlayerConnection* connection )
{
    Q_ASSERT( connection );
    
    if (!m_active)
        goto set_active;
    
    if (m_active->state() == Stopped)
    {
        switch (connection->state())
        {
            case Playing:
                goto set_active;

            case TuningIn:
            case Buffering:
                qWarning() << "Unsupported state for PlayerConnection";
                break;                

            case Stopped:
            case Paused:
                break;
        }
    }
    
    return false;
    
set_active:
    if (!connection) return false;
    m_active = connection;
    emit activeConnectionChanged( connection );
    return true;
}


void 
PlayerMediator::onDestroyed()
{
    m_connections.removeAll( (PlayerConnection*) this->sender() );
}

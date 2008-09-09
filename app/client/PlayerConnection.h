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

#ifndef PLAYER_CONNECTION_H
#define PLAYER_CONNECTION_H

#include "lib/types/Track.h"
#include "PlayerState.h"
#include "PlayerCommandParser.h"


class PlayerConnection
{
    friend class PlayerListener;

    void clear()
    {
        track = Track();
        state = Stopped;
    }    
    
    QString determineName();

public:
    PlayerConnection() : state( Stopped ), command( PlayerCommandParser::Init )
    {}
    
    bool isValid() const
    {
        return !id.isEmpty();
    }
    
    bool operator==( const PlayerConnection& that ) const
    {
        return that.id == this->id;
    }
    
    State state;
    Track track;
    QString id;
    QString name;
    PlayerCommandParser::Command command;
};

#endif

/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Christian Muehlhaeuser, Last.fm Ltd <chris@last.fm>                *
 *      Erik Jaelevik, Last.fm Ltd <erik@last.fm>                          *
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

#ifndef PLAYERCOMMANDS_H
#define PLAYERCOMMANDS_H

#include "lib/moose/TrackInfo.h"

#include <QString>


/// The commands a player plugin can send to the service
enum EPlayerCommand
{
    PCMD_START,
    PCMD_STOP,
    PCMD_PAUSE,
    PCMD_RESUME,
    PCMD_EXIT,  // only used by Service itself
    PCMD_BOOTSTRAP,
    PCMD_INVALID
};


/// The responses we can send back to a player plugin
enum EPlayerResponse
{
    PRESP_OK,
    PRESP_ERROR
};


/*************************************************************************/ /**
    A structure for holding parsed commands. mCmd is the command enum value
    and mPluginId is the id of the player plugin. If the command is a START,
    the track will get parsed to the mTrack object.
******************************************************************************/
class CPlayerCommand
{
public:
    CPlayerCommand()
    {}
    
    CPlayerCommand( EPlayerCommand cmd,
                    const QString& pluginId,
                    const TrackInfo& track )
            : mCmd( cmd ),
              mPluginId( pluginId ),
              mTrack( track )
    {}

    EPlayerCommand mCmd;
    QString        mPluginId;
    TrackInfo      mTrack;    // only used for Start commands
    QString        mUserName; // only used for Bootstrap commands
};

#endif // PLAYERCOMMANDS_H

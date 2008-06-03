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

#ifndef PLAYERCOMMANDPARSER_H
#define PLAYERCOMMANDPARSER_H

#include "PlayerCommands.h"

#include <map>

/*************************************************************************/ /**
    Takes plain text messages from the plugin and parses them into a
    CPlayerCommand for easy digestion by the player connection objects.    
******************************************************************************/
class CPlayerCommandParser
{
public:

    /*********************************************************************/ /**
        Ctor
    **************************************************************************/
    CPlayerCommandParser();

    /*********************************************************************/ /**
        Parses a string received from a plugin.

        @param[in] sCmd String to parse.
        @param[out] cmdOut CPlayerCommand struct to store results in.
    **************************************************************************/
    void
    Parse(
        const std::string& sCmd,
        CPlayerCommand&    cmdOut);


private:

    /*********************************************************************/ /**
        Read the first word of the string and return the corresponding
        command enum.
    **************************************************************************/
    EPlayerCommand
    ParseCommand(
        std::string& sCmd);

    /*********************************************************************/ /**
        Parse arguments off submission string and put them into argsMap.
    **************************************************************************/
    void
    ParseArgs(
        std::string&                 sArgs,
        std::map<char, std::string>& argsMap);
   
    /*********************************************************************/ /**
        Check that all the parameters are present.
    **************************************************************************/
    void
    Validate(
        EPlayerCommand cmd,
        const std::map<char, std::string>& argsMap);
    
    /*********************************************************************/ /**
        Fill TrackInfo.
    **************************************************************************/
    void
    ParseTrack(
        std::map<char, std::string>& argsMap,
        TrackInfo& track);
    
    
    std::map<std::string, EPlayerCommand>   mCmdMap;

};

#endif // PLAYERCOMMANDPARSER_H

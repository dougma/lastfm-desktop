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

#ifndef PLAYER_STATE_H
#define PLAYER_STATE_H

namespace PlayerState
{
    // All states can turn into other states
    // This is KEY there is no implied order
    // Do not add a state if it may have some implied dependence or order
    // instead make an Event
    //NOTE some state transitions may cause multiple events to be emitted
    // eg stopped -> paused will cause a PlaybackStarted, then PlaybackPaused to
    // be emitted
    enum Enum
    {
        Stopped,
        TuningIn,
        Playing,
        Paused,

        TypeMax // leave here pls, kthxbai
    };
}

#endif

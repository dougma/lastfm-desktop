/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole, Erik Jaelevik, 
        Christian Muehlhaeuser

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
#ifndef ITUNESEVENTINTERFACE_H
#define ITUNESEVENTINTERFACE_H

#ifdef WIN32

#include <vector>

/** @author Erik Jalevik <erik@last.fm>
  * @brief This interface should be implemented by anyone interested in receiving
  * events from iTunes without having to touch COM.
  */
class ITunesEventInterface
{
public:

    struct ITunesIdSet
    {
        long sourceId;
        long playlistId;
        long trackId;
        long dbId;
    };

    virtual void
    onDatabaseChanged( std::vector<ITunesIdSet> deletedObjects,
                       std::vector<ITunesIdSet> changedObjects ) = 0;
    
    virtual void
    onPlay( ITunesTrack ) = 0;

    virtual void
    onStop( ITunesTrack ) = 0;

    virtual void
    onTrackChanged( ITunesTrack ) = 0;

    virtual void
    onAboutToPromptUserToQuit() = 0;

    virtual void
    onComCallsDisabled() = 0;

    virtual void
    onComCallsEnabled() = 0;

};

#endif // WIN32

#endif // ITUNESEVENTINTERFACE_H
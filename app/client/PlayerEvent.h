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

#ifndef PLAYER_EVENT_H
#define PLAYER_EVENT_H

/** Usage: connect to qApp->event( int, QVariant ), react to events in your slot
  * Note: if you depend on one object's events being handled before another, you
  * should accept the events at a higher level and control both objects from 
  * there, eg. Container controls all its widgets.
  * NOTE! events do not necessarily dictate state
  * NOTE: some events will always follow each other, see each enum value  for 
  * specification */

/** it may seem strange, to somewhat duplicate states, and that. But all these
  * prove much more useful in practice. Sorry for the quantity though */

namespace PlayerEvent
{
    enum Enum
    {
        /** A playback session is the time inbetween the user starting some 
		  * music and either THE USER stopping the music, or a playlist coming
		  * to a natural end, or playback halting due to some more serious 
		  * issue.
		  *
		  * The point in having an overall PlaybackSession concept is so that
		  * you can know to leave some stuff displayed, even if buffering or
		  * other indeterminate time transitions are occurring.
		  *
		  * Basically show everything until PlaybackSessionEnded happens.
          */
        PlaybackSessionStarted,

		/** The user stopped the playback, or started playing in a different 
		  * player, etc. you get the idea I hope */
        PlaybackSessionEnded,		
		
		/** The next track is being loaded. You should reflect the new track,
		  * although any playing-status-representations should be off.
		  *
		  * If preparing fails, you get a TrackEnded event.
		  *
		  * The track can be a Null track. Which means whatever playback system
		  * we are observing doesn't know what's coming. You should make the GUI
		  * look like it's getting ready to party.
		  */
		PreparingTrack,
        TrackStarted,

		/** You'll get this 30 seconds before the next track starts, if 
		  * possible, you may not get it at all though, so don't depend on it */
		PrepareNextTrack,
		
		TrackEnded,

        /** This will be followed by PlaybackUnpaused or TrackEnded */
        PlaybackPaused,
        /** state will be playing, track will never be empty */
        PlaybackUnpaused,

        /** eg. Radio http buffer is empty, we are rebuffering 
          * you'll get either Unstalled or TrackEnded after this */
        PlaybackStalled,
        /** eg. Radio http rebuffering complete, playback has resumed */
        PlaybackUnstalled,

        /** Note: scrobble submission will not be sent until the end of the
          * track */
        ScrobblePointReached,
        
        /** A new media player has connected to the client, although many 
		  * players can connect at once, we only notify you about one at a time
		  * so when one disconnects we send a connect message for the next one
		  * in the queue */ 
        PlayerConnected,

        /** A known media player has disconnected from the client */
        PlayerDisconnected,

		/** The active player is Last.fm and we're tuning into a station
		  * The data is a RadioStation object.
		  * The station title may be blank, if so bad luck 
		  * TuningIn can occur after TrackEnd and not just after 
		  * PlaybackSessionStarted, as there may be issues getting playlists
		  * from the last.fm radio service be aware of that!
		  */
		TuningIn,
    };
}

#include <QDebug>
inline QDebug operator<<( QDebug d, PlayerEvent::Enum e )
{
	#define _( x ) x: return d << #x
	switch ((PlayerEvent::Enum) e)
	{
		case _(PlayerEvent::PlayerConnected);
		case _(PlayerEvent::PlaybackSessionStarted);
		case _(PlayerEvent::TrackStarted);
		case _(PlayerEvent::PlaybackPaused);
		case _(PlayerEvent::PlaybackUnpaused);
		case _(PlayerEvent::PlaybackStalled);
		case _(PlayerEvent::PlaybackUnstalled);
		case _(PlayerEvent::TrackEnded);
		case _(PlayerEvent::PlaybackSessionEnded);
		case _(PlayerEvent::PlayerDisconnected);
			
		case _(PlayerEvent::PrepareNextTrack);
		case _(PlayerEvent::TuningIn);
		case _(PlayerEvent::ScrobblePointReached);
	}
	#undef _
}

#endif


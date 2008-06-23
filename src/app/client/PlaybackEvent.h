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

#ifndef PLAYBACK_EVENT_H
#define PLAYBACK_EVENT_H

/** Usage: connect to qApp->event( int, QVariant ), react to events in your slot
  * Note: if you depend on one object's events being handled before another, you
  * should accept the events at a higher level and control both objects from 
  * there, eg. Container controls all its widgets.
  * NOTE! events do not necessarily dictate state
  * NOTE: some events will always follow each other, see each enum value  for 
  * specification */

namespace PlaybackEvent
{
    enum Enum
    {
        /** The radio handshake has returned, this contains useful data sadly,
          * so it has its own event */
        Handshaken,

        /** playback has started, data is the playing track 
          * State will be Playing 
          * You will always get a PlaybackEnded event before a non Playback 
          * event is sent, eg. TuningIn, UserChanged */
        PlaybackStarted,

        /** playback continues, data is the new track
          * State will be playing */
        TrackChanged,

        /** the application object requests metadata for all tracks that are played
          * the currentTrack() function will from this point onwards contain the 
          * data for the artist or track */
        ArtistMetaDataAvailable,
        TrackMetaDataAvailable,

        /** playback has ceased, this will not be sent for transitions, eg
          * buffering, retuning, radio playlist fetching, etc.
          * State will be stopped */
        PlaybackEnded,

        /** a player that we listen to paused
          * event will be followed by unpaused or playbackended, even in the case
          * of user-switching occuring or whatever
          * State will be paused */
        PlaybackPaused,
        /** state will be playing */
        PlaybackUnpaused,

        /** eg. Radio http buffer is empty, we are rebuffering 
          * you'll either get Unstalled, TrackChanged or Ended after this
          * State: TODO
          */
        PlaybackStalled,
        /** eg. Radio http rebuffering complete, playback has resumed */
        PlaybackUnstalled,

        /** PlaybackStarted will follow, unless an error occurs, in which case 
          * PlaybackEnded will be sent, despite the lack of playback, this allows
          * you to keep the UI in the right state */
        TuningIn,

        /** Note: scrobble submission will not be sent until the end of the
          * track */
        ScrobblePointReached,

        TypeMax /** leave at end of enum, kthxbai */
    };
}

#endif

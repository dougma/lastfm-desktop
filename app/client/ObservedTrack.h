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

#ifndef OBSERVED_TRACK_H
#define OBSERVED_TRACK_H

#include "lib/unicorn/Track.h"
#include "StopWatch.h" // sadly necessary dep
#include <QPointer>


class ObservedTrack : public Track
{
    QPointer<StopWatch> m_watch;

    friend class PlayerManager;

public:
    ObservedTrack() // QMetaType needs this :(
    {
        // initialising m_watch to 0 is done by QPointer for us
    }

    ObservedTrack( const Track& that )
    {
        static_cast<Track&>(*this) = that;
    }

    /** If you copy the returned object use a QPointer, as it is liable to be
      * deleted after the track is finished. This is by design, since you don't 
      * want to reflect the information about a track that no longer exists */
    StopWatch* watch() const { return m_watch; }

    /** @returns the time in seconds when the track is considered scrobbled */
    uint scrobblePoint() const;
};


#include <QMetaType>
Q_DECLARE_METATYPE( ObservedTrack );

#endif

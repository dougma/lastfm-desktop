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

#include "ObservedTrack.h"
#include "lib/moose/MooseSettings.h"


uint
ObservedTrack::scrobblePoint() const
{
    // If we don't have a length or it's less than the minimum, return the
    // threshold
    if ( duration() <= 0 || duration() < int(kScrobbleMinLength) )
        return kScrobbleTimeMax;

    float scrobPoint = qBound( int(kScrobblePointMin),
        Moose::Settings().scrobblePoint(),
        int(kScrobblePointMax) );
    scrobPoint /= 100.0f;

    return qMin( int(kScrobbleTimeMax), int( duration() * scrobPoint ) );
}

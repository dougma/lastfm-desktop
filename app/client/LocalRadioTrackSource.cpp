/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

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
#include "LocalRadioTrackSource.h"
#include "LocalRql.h"
#include <lastfm/WsError>


///////////////////////////////////////////////////////////////////////////


LocalRadioTrackSource::LocalRadioTrackSource(LocalRqlResult* rqlResult)
: m_rqlResult(rqlResult)
, m_waiting(false)
, m_endReached(false)
{
    Q_ASSERT( rqlResult );
    connect( rqlResult, SIGNAL(track(Track)), SLOT(onTrack(Track)) );
    connect( rqlResult, SIGNAL(endOfTracks()), SLOT(onEndOfTracks()) );
}

LocalRadioTrackSource::~LocalRadioTrackSource()
{
    delete m_rqlResult;
}

Track
LocalRadioTrackSource::takeNextTrack()
{
    // always try to have a track ready to
    // prevent "Tuning in" between tracks
    if (!m_endReached) {
        m_rqlResult->getNextTrack();
    }
    
    Track t;
    if (m_buffer.size()) {
        t = m_buffer.takeFirst();
    } else if (!m_waiting) {
        if (m_endReached) {
            emit error(Ws::NotEnoughContent);
        }             
        m_waiting = true;
    }
    return t;
}

void 
LocalRadioTrackSource::start()
{
    takeNextTrack();
}

void
LocalRadioTrackSource::onTrack(Track t)
{
    // source decided by rgarrett 2008-12-09:
    MutableTrack(t).setSource(Track::PersonalisedRecommendation);
    
    m_buffer += t;
    if (m_waiting) {
        m_waiting = false;
        emit trackAvailable();
    }
}

void
LocalRadioTrackSource::onEndOfTracks()
{
    m_endReached = true;
    if (m_waiting) {
        emit error(Ws::NotEnoughContent);
    } 
}

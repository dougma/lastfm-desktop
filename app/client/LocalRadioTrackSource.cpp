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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "LocalRadioTrackSource.h"
#include "LocalRql.h"
#include "lib/lastfm/ws/WsError.h"


///////////////////////////////////////////////////////////////////////////


LocalRadioTrackSource::LocalRadioTrackSource(LocalRqlResult* rqlResult)
: m_rqlResult(rqlResult)
{
    Q_ASSERT(rqlResult);
    // QueuedConnections are important here, see LocalRql.h
    connect(rqlResult, SIGNAL(track(Track)), SLOT(onTrack(Track)), Qt::QueuedConnection);
    connect(rqlResult, SIGNAL(endOfTracks()), SLOT(onEndOfTracks()), Qt::QueuedConnection);
}

LocalRadioTrackSource::~LocalRadioTrackSource()
{
    delete m_rqlResult;
}

Track
LocalRadioTrackSource::takeNextTrack()
{
    if (m_buffer.size()) {
        return m_buffer.takeFirst();
    }

    m_rqlResult->getNextTrack();
    return Track();
}

void 
LocalRadioTrackSource::start()
{
    takeNextTrack();
}

void
LocalRadioTrackSource::onTrack(Track t)
{
    m_buffer.push_back(t);
    emit trackAvailable();
}

void
LocalRadioTrackSource::onEndOfTracks()
{
    emit error( Ws::NotEnoughContent );
}


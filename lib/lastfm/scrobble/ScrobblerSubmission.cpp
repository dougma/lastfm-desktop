/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
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

#include "ScrobblerSubmission.h"
#include "ScrobbleCache.h"
#include "Scrobbler.h"
#include "Scrobble.h"

using lastfm::Track;


void
ScrobblerSubmission::setTracks( const QList<Track>& tracks )
{
    m_tracks = tracks;
    // submit in chronological order
    qSort( m_tracks.begin(), m_tracks.end() );
}


void
ScrobblerSubmission::submitNextBatch()
{
    if (isActive())
		// the tracks cannot be submitted at this time
		// if a parent Scrobbler instance exists, it will submit another batch
		// when the current one is done
		return;

    m_batch.clear(); //yep before isEmpty() check
    m_data.clear();

    if (m_tracks.isEmpty())
        return;

    bool portable = false;
    for (int i = 0; i < 50 && !m_tracks.isEmpty(); ++i)
    {
        Scrobble s = m_tracks.takeFirst();

        QByteArray const N = QByteArray::number( i );
        #define e( x ) QUrl::toPercentEncoding( x )
        m_data += "&a[" + N + "]=" + e(s.artist()) +
                  "&t[" + N + "]=" + e(s.title()) +
                  "&i[" + N + "]=" + QByteArray::number( s.timestamp().toTime_t() ) +
                  "&o[" + N + "]=" + s.sourceString() +
                  "&r[" + N + "]=" + s.ratingCharacter() +
                  "&l[" + N + "]=" + QByteArray::number( s.duration() ) +
                  "&b[" + N + "]=" + e(s.album()) +
                  "&n[" + N + "]=" + QByteArray::number( s.trackNumber() ) +
                  "&m[" + N + "]=" + e(s.mbid());
        #undef e

        if (s.source() == Track::MediaDevice)
            portable = true;

        m_batch += s;
    }

    if (portable)
        m_data += "&portable=1";

    request();
};

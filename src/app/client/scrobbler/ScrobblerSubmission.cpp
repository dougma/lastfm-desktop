/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd                                       *
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
#include "version.h"


void
ScrobblerSubmission::request()
{
    //TODO if (!canSubmit()) return;
    if (m_batch.size()) return;

    ScrobbleCache cache( manager()->username() );
    QList<TrackInfo> tracks = cache.tracks();

    if (tracks.isEmpty())
        return;

    // we need to put the tracks in chronological order or the Scrobbling Service
    // rejects the ones that are later than previously submitted tracks
    // this is only relevent if the cache is greater than 50 in size as then
    // submissions are done in batches, but better safe than sorry
    //TODO sort in the persistent cache
    qSort( tracks.begin(), tracks.end(), TrackInfo::lessThan );
    tracks = tracks.mid( 0, 50 );
    m_batch = tracks;

    Q_ASSERT( manager()->session().size() );
    Q_ASSERT( tracks.size() <= 50 );

    //////
    QString data = "s=" + manager()->session();
    bool portable = false;
    int n = 0;

    foreach (TrackInfo const i, tracks)
    {
        QString const N = QString::number( n++ );
        #define e( x ) QUrl::toPercentEncoding( x )
        data += "&a[" + N + "]=" + e(i.artist()) +
                "&t[" + N + "]=" + e(i.track()) +
                "&i[" + N + "]=" + QString::number( i.timeStamp() ) +
                "&o[" + N + "]=" + i.sourceString() +
                "&r[" + N + "]=" + i.ratingCharacter() +
                "&l[" + N + "]=" + e(QString::number( i.duration() )) +
                "&b[" + N + "]=" + e(i.album()) +
                "&n[" + N + "]=" + //position in album if known, and we don't generally
                "&m[" + N + "]=" + i.mbId();
        #undef e

        if (i.source() == TrackInfo::MediaDevice)
            portable = true;
    }

    if (portable)
        data += "&portable=1";

    m_data = data.toUtf8();
    ScrobblerPostHttp::request();
};

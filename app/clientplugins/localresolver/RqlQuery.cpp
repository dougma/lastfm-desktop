/***************************************************************************
 *   Copyright 2007-2009 Last.fm Ltd.                                      *
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

#include "RqlQuery.h"
#include "RqlQueryThread.h"
#include "LocalCollection.h"
#include "MediaMetaInfo.h"
#include <QUrl>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#define ARTIST_HISTORY_LIMIT 4


extern QString remapVolumeName(const QString& volume);
extern TrackResult sample(const ResultSet& rs, boost::function<float(uint, uint)> pushdown);


RqlQuery::RqlQuery(RqlQueryThread* queryThread, ResultSet tracks)
:m_queryThread(queryThread)
,m_tracks(tracks)
{
}


unsigned 
RqlQuery::tracksLeft()
{
    return m_tracks.size();
}

void
RqlQuery::getNextTrack(ILocalRqlTrackCallback* cb)
{
    Q_ASSERT(cb);
    if (cb) {
        m_queryThread->enqueueGetNextTrack(this, cb);
    }
}

void
RqlQuery::getNextTrack(LocalCollection& collection, ILocalRqlTrackCallback* cb, QSet<uint>& recentTracks)
{
    // loop until we get a sample from the resultset for 
    // which we can read id3 tags:

    while (m_tracks.size()) {
        TrackResult tr( 
            sample( 
                m_tracks, 
                boost::bind( 
                    &RqlQuery::pushdownFactor, this, 
                    recentTracks, _1, _2 ) ) );

        bool removed = m_tracks.remove(tr);
        Q_ASSERT(removed);

        LocalCollection::FileResult result;
        if (collection.getFileById(tr.trackId, result)) {
            QString filename(remapVolumeName(result.m_sourcename) + result.m_path + result.m_filename);

            // read id3 tags from the file
            // todo: check that they kinda match what we have in our db?
            MediaMetaInfo* mmi = MediaMetaInfo::create(filename);
            if (mmi) {
                cb->trackOk(
                    mmi->title().toUtf8(), // result.m_title.toUtf8(),
                    mmi->album().toUtf8(), // result.m_album.toUtf8(),
                    mmi->artist().toUtf8(), // result.m_artist.toUtf8(),
                    QUrl::fromLocalFile(filename).toString().toUtf8(), 
                    mmi->duration());  //result.m_duration);

                recentTracks.insert( tr.trackId );

                // remember a fixed (and small) number of recent artists
                if (m_artistHistory.size() == ARTIST_HISTORY_LIMIT ) {
                    m_artistHistory.dequeue();
                }
                m_artistHistory.enqueue( tr.artistId );
                return;
            }
        }
    }
    cb->trackFail();
}

float 
RqlQuery::pushdownFactor(const QSet<uint>& recentTracks, uint artistId, uint trackId)
{
    float f = m_artistHistory.contains( artistId ) ? 0.001f : 1.0f;
    f *= recentTracks.contains( trackId ) ? 0.001f : 1.0f;
    return f;
}

void
RqlQuery::finished()
{
    m_queryThread->enqueueDelete(this);
}

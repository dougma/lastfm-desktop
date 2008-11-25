/***************************************************************************
 *   Copyright 2007-2008 Last.fm Ltd.                                      *
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
#include "LocalCollection.h"
#include <QUrl>

extern QString remapVolumeName(const QString& volume);


RqlQuery::RqlQuery(LocalCollection& lc, QSet<uint> tracks)
:m_collection(lc)
,m_tracks(tracks)
{
}


unsigned 
RqlQuery::tracksLeft()
{
    return m_tracks.size();
}

bool 
RqlQuery::nextTrack(ILocalRqlTrackCallback* cb)
{
    Q_ASSERT(cb);
    if (cb) {
        while (m_tracks.size()) {
            int fileId = *m_tracks.begin();
            m_tracks.remove(fileId);

            LocalCollection::FileResult result;
            if (m_collection.getFileById(fileId, result)) {
                cb->title(result.m_title.toUtf8());
                cb->album(result.m_album.toUtf8());
                cb->artist(result.m_artist.toUtf8());
                cb->duration(result.m_duration);
                cb->url(
                    QUrl::fromLocalFile(
                        remapVolumeName(result.m_sourcename) + result.m_path + result.m_filename).toEncoded() );
                return true;
            }
        }
    }
    return false;
}

void
RqlQuery::finished()
{
    delete this;
}


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

#include "SimilarArtists.h"
#include "similarity/CosSimilarity.h"
#include <boost/bind.hpp>



bool operator<(TagDataset::ArtistId i, const TagDataset::Entry& e)
{
    return i < e.artistId;
}

bool operator<(const TagDataset::Entry& e, TagDataset::ArtistId i)
{
    return e.artistId < i;
}


void
resultCb(QList<SimilarArtists::Result>& results, const TagDataset::Entry& entry, float score)
{
    if (score > 0.1) {
        results.push_back(qMakePair(entry.artistId, score));
    }
}

QList<SimilarArtists::Result>
SimilarArtists::similarArtists(LocalCollection& coll, int artistId)
{
    QList<Result> results;

    m_dataset.load(coll);
    TagDataset::Entry *pArtist = m_dataset.findArtist(artistId);

    if (pArtist && m_dataset.m_allArtists.size()) {
        if (m_dataset.m_allArtists.size()) {
            moost::algo::CosSimilarity::findSimilar/*<TagDataset::Entry, TagDataset::EntryList, TagDataset>*/(
                boost::bind(resultCb, boost::ref(results), _1, _2),
                *pArtist,
                m_dataset.m_allArtists,
                m_dataset);
        }
    }

    return results;
}

bool operator<(const SimilarArtists::Result& a, const SimilarArtists::Result& b)
{
    // yes, we want it in "reverse" order, ie: bigger things at the top.
    return a.second > b.second;
}


ResultSet
SimilarArtists::filesBySimilarArtist(LocalCollection& coll, const char* artist)
{
    int artistId = coll.getArtistId(QString(artist).simplified().toLower(), false);

    QList<SimilarArtists::Result> artistList = similarArtists(coll, artistId);

    ResultSet result;
    qSort(artistList.begin(), artistList.end());

    QList<SimilarArtists::Result>::iterator pArtist = artistList.begin();
    int artistCount = 0;
    int trackCount = 0;
    while ((trackCount < 10000 || artistCount < 20) && pArtist != artistList.end())
    {
        if (pArtist->first != artistId) {
            QList<uint> tracks = coll.filesByArtistId(pArtist->first);
            foreach(uint trackId, tracks) {
                TrackResult tr;
                tr.trackId = trackId;
//                tr.artistId =  pArtist->first;
                tr.weight = pArtist->second;
                result << tr;
            }
            trackCount += tracks.size();
            artistCount++;
        }
        pArtist++;
    }

    return result;
}
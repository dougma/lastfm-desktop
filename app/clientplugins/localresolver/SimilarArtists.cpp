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





bool operator<(TagDataset::ArtistId i, const TagDataset::Entry& e)
{
    return i < e.first;
}

bool operator<(const TagDataset::Entry& e, TagDataset::ArtistId i)
{
    return e.first < i;
}


void
SimilarArtists::similarArtists(LocalCollection& coll, const char* artist)
{
    int artistId = coll.getArtistId(QString(artist).simplified().toLower(), false);

    typedef std::pair<TagDataset::Entry, float> Result;
    std::vector<Result> results;

    m_dataset.load(coll);
    TagDataset::Entry *pArtist = m_dataset.findArtist(artistId);

    if (pArtist && m_dataset.m_allArtists.size()) {
        if (m_dataset.m_allArtists.size()) {
            moost::algo::CosSimilarity::findSimilar/*<TagDataset::Entry, TagDataset::EntryList, TagDataset>*/(
                results,
                *pArtist,
                m_dataset.m_allArtists,
                m_dataset,
                0.0);       // minsim
        }
    }



    foreach (const Result& result, results) {
        int artistId = result.first.first;
        float score = result.second;

        int ii = 0;
    }
}


ResultSet
SimilarArtists::filesBySimilarArtist(LocalCollection& coll, const char* artist)
{
    similarArtists(coll, artist);

    return ResultSet();
}
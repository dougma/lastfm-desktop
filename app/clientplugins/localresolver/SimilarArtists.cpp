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
#include "lib/lastfm/types/Tag.h"
#include "lib/lastfm/types/Artist.h"
#include "lib/lastfm/ws/WsReplyBlock.h"
#include "similarity/CosSimilarity.h"
#include <boost/bind.hpp>
#include <QtAlgorithms>


// these two operators are for qBinaryFind in TagDataset::findArtist

bool 
operator<(TagDataset::ArtistId i, const TagDataset::Entry& e)
{
    return i < e.artistId;
}

bool 
operator<(const TagDataset::Entry& e, TagDataset::ArtistId i)
{
    return e.artistId < i;
}


//


bool
orderByWeightDesc(const WeightedString& a, const WeightedString& b)
{
    return a.weighting() > b.weighting();
}


void
dlArtistTags(LocalCollection& coll, const QString& artist, TagDataset::Entry& outTags)
{
    WsReply* reply = WsReplyBlock::wait(
        Artist(artist).getTopTags(),
        10 * 1000);

    if (reply) {
        WeightedStringList wsl = Tag::list(reply);
        qSort(wsl.begin(), wsl.end(), orderByWeightDesc);

        // take the top ten tags having non-zero weight
        int count = 0;
        WeightedStringList::iterator p = wsl.begin();
        while (p != wsl.end() && p->weighting() > 0  && count < 10) {
            TagDataset::TagId tagId = coll.getTagId(*p, LocalCollection::Create);
            // webservice gives weight 0..100; we want it 0..1
            TagDataset::TagWeight tagWeight = p->weighting() / 100.0;     
            outTags.tagVec.push_back( qMakePair(tagId, tagWeight) );
            p++;
            count++;
        }
        outTags.norm = TagDataset::calcNormal(outTags.tagVec);
    }
}


//////////////////////////////////////


void
resultCb(QList<SimilarArtists::Result>& results, const TagDataset::Entry& entry, float score)
{
    if (score > 0.1) {
        results.push_back(qMakePair(entry.artistId, score));
    }
}

QList<SimilarArtists::Result>
SimilarArtists::getSimilarArtists(LocalCollection& coll, const char* artist, int artistId)
{
    QList<Result> results;

    TagDataset::Entry otherArtistTags;
    TagDataset::Entry *pArtist = 0;

    {
        m_dataset.load(coll);
        pArtist = m_dataset.findArtist(artistId);

        if (pArtist == 0) {
            otherArtistTags.artistId = artistId;
            dlArtistTags(coll, QString(artist), otherArtistTags);
            if (otherArtistTags.tagVec.size()) {
                pArtist = &otherArtistTags;
            }
        }
    }

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


bool 
artistList_orderByWeightDesc(const SimilarArtists::Result& a, const SimilarArtists::Result& b)
{
    return a.second > b.second;
}

ResultSet
SimilarArtists::filesBySimilarArtist(LocalCollection& coll, const char* artist)
{
    ResultSet result;

    int artistId = coll.getArtistId(
        QString(artist).simplified().toLower(), 
        LocalCollection::Create);

    QList<SimilarArtists::Result> artistList = getSimilarArtists(coll, artist, artistId);
    qSort(artistList.begin(), artistList.end(), artistList_orderByWeightDesc);

    QList<SimilarArtists::Result>::iterator pArtist = artistList.begin();
    int artistCount = 0;
    int trackCount = 0;
    while ((trackCount < 10000 || artistCount < 20) && pArtist != artistList.end())
    {
        if (pArtist->first != artistId) {
            QList<uint> tracks = coll.filesByArtistId(pArtist->first, LocalCollection::AvailableSources);
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

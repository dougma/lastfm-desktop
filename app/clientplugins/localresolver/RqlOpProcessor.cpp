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


#include "RqlOpProcessor.h"
#include "rqlparser/enums.h"
#include "LocalCollection.h"
#include "SimilarArtists.h"


using namespace fm::last::query_parser;


RqlOpProcessor::RqlOpProcessor(QList<RqlOp> &ops, LocalCollection& collection, SimilarArtists& similarArtists)
    : m_collection(collection)
    , m_similarArtists(similarArtists)
    , m_it(ops.begin())
    , m_end(ops.end())
{
}


void 
RqlOpProcessor::next()
{
    if (++m_it == m_end) {
        throw "unterminated query. how could the parser do this to us??"; // it's an outrage
    }
}


ResultSet 
RqlOpProcessor::process()
{
    if (m_it->isRoot) {
        // note the operator type, then move on and get the operands
        int op = m_it->type;
        ResultSet a( (next(), process()) );
        ResultSet b( (next(), process()) );
        switch (op) {
            case OT_AND:
                return a.and(b);

            case OT_OR:
                return a.or(b);

            case OT_AND_NOT:
                return a.and_not(b);
        }
        throw "unknown operation";
    }

    // it's a leaf node, or a 'source' if you like
    switch (m_it->type) {
        case RS_LIBRARY:
        case RS_LOVED:
        case RS_NEIGHBORS:
        case RS_PLAYLIST:
        case RS_RECOMMENDED:
        case RS_GROUP:
        case RS_EVENT:
            return unsupported();

        case RS_SIMILAR_ARTISTS:
            return similarArtist();

        case RS_GLOBAL_TAG:
            return globalTag();

        case RS_USER_TAG:
            return userTag();

        case RS_ARTIST:
            return artist();
    }

    throw "unknown field";
}

ResultSet 
RqlOpProcessor::unsupported()
{
    return UnsupportedResultSet();
}

ResultSet 
RqlOpProcessor::globalTag()
{
    typedef QPair<uint, float> TrackPair;

    QList<TrackPair> tracks(m_collection.filesWithTag(m_it->name.data(), LocalCollection::AvailableSources));

    ResultSet rs;
    foreach(const TrackPair& tp, tracks) {
        TrackResult tr;
        tr.trackId = tp.first;
        tr.weight = tp.second;
        rs << tr;
    }
    normalise(m_it->weight, rs);
    return rs;
}

ResultSet 
RqlOpProcessor::userTag()
{
    // todo: we have no user tags yet
    return globalTag();
}

ResultSet 
RqlOpProcessor::artist()
{
    QList<uint> tracks(m_collection.filesByArtist(m_it->name.data(), LocalCollection::AvailableSources));
    ResultSet rs;
    int count = tracks.size();
    if (count) {
        float weight = m_it->weight / count;
        foreach(uint id, tracks) {
            TrackResult tr;
            tr.trackId = id;
            tr.weight = weight;
            rs << tr;
        }
        normalise(m_it->weight, rs);
    }
    return rs;
}

ResultSet 
RqlOpProcessor::similarArtist()
{
    ResultSet rs( m_similarArtists.filesBySimilarArtist(m_collection, m_it->name.data()) );
    normalise(m_it->weight, rs);
    return rs;
}

// static
ResultSet 
RqlOpProcessor::process(QList<RqlOp> &ops, LocalCollection& collection, SimilarArtists& similarArtists)
{
    return RqlOpProcessor(ops, collection, similarArtists).process();
}

void
RqlOpProcessor::normalise(float weight, ResultSet& rs)
{
    if (weight < 0.0001) return;

    float sum = 0;
    foreach(const TrackResult& tr, rs) {
        sum += tr.weight;
    }
    sum /= weight;
    if (sum < 0.0001) return;

    for(ResultSet::iterator p = rs.begin(); p != rs.end(); p++) {
        const_cast<TrackResult*>(&(*p))->weight /= sum;
    }
}

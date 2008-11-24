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


#include "RqlOpProcessor.h"
#include "rqlparser/enums.h"
#include "LocalCollection.h"
#include "SimilarArtists.h"

using namespace fm::last::query_parser;


RqlOpProcessor::RqlOpProcessor(std::vector<RqlOp> &ops, LocalCollection& collection, SimilarArtists& similarArtists)
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
    return m_collection.filesWithTag(m_it->name.data());
}

ResultSet 
RqlOpProcessor::userTag()
{
    return m_collection.filesWithTag(m_it->name.data());
}

ResultSet 
RqlOpProcessor::artist()
{
    return m_collection.filesByArtist(m_it->name.data());
}

ResultSet 
RqlOpProcessor::similarArtist()
{
    return m_similarArtists.filesBySimilarArtist(m_collection, m_it->name.data());
}

// static
ResultSet 
RqlOpProcessor::process(std::vector<RqlOp> &ops, LocalCollection& collection, SimilarArtists& similarArtists)
{
    return RqlOpProcessor(ops, collection, similarArtists).process();
}

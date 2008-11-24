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

#ifndef SIMILAR_ARTISTS_H
#define SIMILAR_ARTISTS_H

#include <QVector>
#include <QPair>
#include <QList>
#include "LocalCollection.h"
#include "ResultSet.h"
#include <math.h>


struct TagDataset
{
    typedef int ArtistId;
    typedef int TagId;
    typedef float TagWeight;
    typedef QVector< QPair< TagId, TagWeight > > TagVec;
    typedef QPair< ArtistId, TagVec > Entry;
    typedef QList< Entry > EntryList;
    
    //////////////////////////////////////////

    EntryList m_allArtists;

    bool load(LocalCollection& collection)
    {
        if (0 == m_allArtists.size()) {
            m_allArtists = collection.allTags();
        }

        return true;
    }

    Entry* findArtist(int artistId)
    {
        EntryList::iterator it = qBinaryFind(m_allArtists.begin(), m_allArtists.end(), artistId);
        return it == m_allArtists.end() ? 0 : &(*it);
    }

    //////////////////////////////////////////////////
    // these methods fulfil the policy requirements 
    // of the findSimilar template function

    TagVec::const_iterator get_begin(const Entry& entry) const
    {
        return entry.second.begin(); 
    }

    TagVec::const_iterator get_end(const Entry& entry) const
    { 
        return entry.second.end(); 
    }

    int get_id(const TagVec::const_iterator& it) const
    { 
        return it->first; 
    }

    float get_score(const TagVec::const_iterator& it) const
    { 
        return it->second; 
    }

    float get_norm(const Entry& entry) const
    { 
        float sum = 0;
        typedef QPair< TagId, TagWeight > Tag;
        foreach(const Tag& tag, entry.second) {
            sum += (tag.second * tag.second);
        }
        return sqrt(sum); 
    }

    double apply_post_process( const Entry& entryA, const Entry& entryB, 
                              const std::pair<double, int>& simVal) const
    { 
        if ( simVal.second < 5 )
            return simVal.first * ( static_cast<double>(simVal.second) / 5 );
        else
            return simVal.first;
    }

};

bool operator<(TagDataset::ArtistId i, const TagDataset::Entry& e);
bool operator<(const TagDataset::Entry& e, TagDataset::ArtistId i);


class SimilarArtists
{
    TagDataset m_dataset;

    void similarArtists(LocalCollection& coll, const char *artist);

public:
    ResultSet filesBySimilarArtist(LocalCollection& coll, const char *artist);
};


#endif
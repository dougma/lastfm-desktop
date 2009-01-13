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


#ifndef RESULT_SET_H
#define RESULT_SET_H

#include <QSet>

struct TrackResult
{
    uint trackId;
//    uint artistId;
    float weight;

    bool operator==(const TrackResult& that) const
    {
        return this->trackId == that.trackId;
    }
};

extern uint qHash(const TrackResult& t);


class ResultSet : public QSet<TrackResult>
{
    // marks a special kind of result set which 
    // has come from an unsupported rql service name.
    // it behaves differently depending on the operation
    // so as not to ruin the whole query.  :)

protected:
    bool m_unsupported; 

public:
    ResultSet()
        :m_unsupported(false)
    {
    }

    // intersect the other ResultSet with this one. 
    ResultSet and(const ResultSet &other)
    {
        intersect(other);
        return *this;
    }

    // 'unite' the other ResultSet with this one. 
    // the weights are added when a TrackResult appears in both sets.
    ResultSet or(const ResultSet &other)
    {
        ResultSet others;
        foreach(const TrackResult& t, other)
        {
            ResultSet::iterator it = find(t);
            if (it == end()) {
                others.insert(t);
            } else {
                // operator* gives a const reference, but we're
                // not modifiing the trackId (the source of the hash)
                // so we're safe to cast it away
                const_cast<TrackResult*>(&(*it))->weight += t.weight;
            }
        }

        unite(others);
        return *this;
    }

    ResultSet and_not(const ResultSet &other)
    {
        subtract(other);
        return *this;
    }

};


struct UnsupportedResultSet : public ResultSet
{
    UnsupportedResultSet()
    {
        m_unsupported = true;
    }
};




#endif
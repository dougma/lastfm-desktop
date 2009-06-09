/*
   Copyright 2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "Shuffler.h"

Shuffler::Shuffler(QObject* parent /* = 0 */)
: QObject(parent)
, m_artistHistorySize(4)
{
}

BoffinPlayableItem 
Shuffler::sampleOne()
{
    BoffinPlayableItem result = sample();
    if (result.isValid()) {
        m_artistHistory.push_back(result.artist());
        while (m_artistHistory.size() > m_artistHistorySize) {
            m_artistHistory.pop_front();
        }
    }
    return result;
}

const Shuffler::ItemList& 
Shuffler::items()
{
    return m_items;
}

void 
Shuffler::setArtistHistorySize(unsigned size)
{
    m_artistHistorySize = size;
}

void
Shuffler::clear()
{
    m_items.clear();
}

bool orderByWorkingWeightDesc(const BoffinPlayableItem& a, const BoffinPlayableItem& b)
{
    return a.workingweight() > b.workingweight();
}

// pull out a single item
BoffinPlayableItem
Shuffler::sample()
{
    BoffinPlayableItem result;

    if (m_items.size()) {
        ItemList::iterator begin = m_items.begin();
        ItemList::iterator end = m_items.end();
        ItemList::iterator it;

        // reweight:
        float totalWeight = 0;
        for (it = begin; it != end; it++) {
            it->workingweight() = it->weight() * pushdown(*it);
            totalWeight += it->workingweight();
        }
        // normalise weights, sum to 1
        for (it = begin; it != end; it++) {
            it->workingweight() /= totalWeight;
        }
        qSort(begin, end, orderByWorkingWeightDesc);

        ItemList::iterator sample = m_sampler.singleSample(begin, end, true);
        result = *sample;
        m_items.removeAt(sample - begin);
    }

    return result;
}


float 
Shuffler::pushdown(const BoffinPlayableItem& item)
{
    return m_artistHistory.contains(item.artist(), Qt::CaseInsensitive) ? 0.00001 : 1.0;
}

void 
Shuffler::receivePlayableItem(BoffinPlayableItem item)
{
    m_items.push_back(item);
}
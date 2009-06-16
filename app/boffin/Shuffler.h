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
#ifndef SHUFFLER_H
#define SHUFFLER_H

#include <QStringList>
#include "playdar/BoffinPlayableItem.h"
#include "sample/SampleFromDistribution.h"


class Shuffler : public QObject
{
    Q_OBJECT

public:
    typedef QList<BoffinPlayableItem> ItemList;

    Shuffler(QObject* parent = 0);
    BoffinPlayableItem sampleOne();
    const ItemList& items();
    void setArtistHistorySize(unsigned size);
    void setSongHistorySize(unsigned size);
    void clear();
    void clearHistory();

public slots:
    void receivePlayableItem(BoffinPlayableItem item);

private:
    BoffinPlayableItem sample();
    void result(const BoffinPlayableItem& item);
    float pushdown(const BoffinPlayableItem& item);
    float pushdownSong(const BoffinPlayableItem& item);

    struct AccessPolicy
    {
        float operator()(const QList<BoffinPlayableItem>::const_iterator& el) const
        { 
            return el->workingweight(); 
        }
    };

    struct CopyPolicy
    {
       template <typename IT>
       BoffinPlayableItem operator()(IT& it) const
       {
          return *it;
       }

       template <typename IT>
       const BoffinPlayableItem operator()(const IT& it) const
       {
          return *it;
       }
    };


    fm::last::algo::ListBasedSampler<AccessPolicy, CopyPolicy> m_sampler;
    ItemList m_items;               // items arrive here
    QStringList m_artistHistory;
    int m_artistHistorySize;
    QList<BoffinPlayableItem> m_songHistory;
    int m_songHistorySize;
};

#endif

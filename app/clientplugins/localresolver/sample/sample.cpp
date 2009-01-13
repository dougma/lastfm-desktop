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

#include <QVector>
#include "../ResultSet.h"

#include "SampleFromDistribution.h"

using namespace fm::last::algo;


struct AccessPolicy
{
  template <typename IT>
  float operator()(const IT& el) const
  { return el->weight; }
};


struct CopyPolicy
{
   //template <typename InIT, typename OutIT>
   //void operator() ( const InIT& from, OutIT& to ) const
   //{
   //   *to = *from;
   //}

   template <typename IT>
   typename std::iterator_traits<IT>::value_type operator() ( IT& it ) const
   {
      return *it;
   }

   template <typename IT>
   const typename std::iterator_traits<const IT>::value_type operator() ( const IT& it ) const
   {
      return *it;
   }
};


bool orderByWeightDesc(const TrackResult& a, const TrackResult& b)
{
    return a.weight > b.weight;
}

TrackResult
sample(ResultSet rs)
{
    // convert the ResultSet to a vector for sorting
    std::vector<TrackResult> tracks;
    float totalWeight = 0;
    foreach (const TrackResult& tr, rs) {
        tracks.push_back(tr);
        totalWeight += tr.weight;
    }

    std::vector<TrackResult>::iterator pBegin = tracks.begin();
    std::vector<TrackResult>::iterator pEnd = tracks.end();

    std::sort(pBegin, pEnd, orderByWeightDesc);

    // normalise weights, sum to 1
    std::vector<TrackResult>::iterator pIt = tracks.begin();
    for (; pIt != pEnd; pIt++) {
        pIt->weight /= totalWeight;
    }

    // pull out a single sample
    ListBasedSampler<AccessPolicy, CopyPolicy> sampler;
    return sampler.singleSample(pBegin, pEnd, true);
}


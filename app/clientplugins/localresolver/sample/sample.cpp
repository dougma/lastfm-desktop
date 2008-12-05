#include <QVector>
#include "../ResultSet.h"

#include "SampleFromDistribution.h"

using namespace fm::last::algo;


struct AccessPolicy
{
  template <typename IT>
  float operator()(const IT& el) const
  { return el->second; }
};


struct CopyPolicy
{
   template <typename InIT, typename OutIT>
   void operator() ( const InIT& from, OutIT& to ) const
   {
      *to = from->first;
   }

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


bool orderByWeightDesc(const std::pair<uint, float>& a, const std::pair<uint, float>& b)
{
    return a.second > b.second;
}

QVector<uint>
sample(ResultSet rs, unsigned playlistSize)
{
    // convert the ResultSet to a vector for sorting
    std::vector<std::pair<uint, float>> tracks;
    float totalWeight = 0;
    foreach (const TrackResult& tr, rs) {
        tracks.push_back(std::make_pair(tr.trackId, tr.weight));
        totalWeight += tr.weight;
    }

    if (tracks.size() < playlistSize)
        playlistSize = tracks.size();

    QVector<uint> result;
    result.resize(playlistSize);
    QVector<uint>::iterator pResult = result.begin();

    /////

    std::vector<std::pair<uint, float>>::iterator pBegin = tracks.begin();
    std::vector<std::pair<uint, float>>::iterator pEnd = tracks.end();

    std::sort(pBegin, pEnd, orderByWeightDesc);

    // normalise weights, sum to 1
    std::vector<std::pair<uint, float>>::iterator pIt = tracks.begin();
    for (; pIt != pEnd; pIt++) {
        pIt->second /= totalWeight;
    }

    ListBasedSampler<AccessPolicy, CopyPolicy> sampler;
    sampler.multiSample(pBegin, pEnd, pResult, playlistSize, true);

    return result;
}


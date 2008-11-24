#ifndef __COS_SIMILARITY_H
#define __COS_SIMILARITY_H

#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>

// -----------------------------------------------------------------------------

namespace moost { namespace algo {

class CosSimilarity
{

public:

   // TEntryAccessPolicy has to implement
   //
   // TEntryIterator get_begin(const TEntry&)
   // TEntryIterator get_end(const TEntry&)
   // (IMPORTANT: the iterator range returned MUST be sorted by id!)
   //
   // int get_id(const TEntryIterator&)
   // float get_score(const TEntryIterator&)
   //
   // float get_norm(const TEntry&)
   // Note: here the pair contains the value from the similarity computation and the number of co-occurences
   // double apply_post_process( const TEntry& entryA, const TEntry& entryB, const std::pair<double, int>& simVal ); 
   //
   // note: the EntryAccessPolicy can contain other stuff as it is always copied by ref and
   //       treated as const
   template< typename TEntry, typename TCandContainer, typename TEntryAccessPolicy>
   static void findSimilar( std::vector< std::pair<TEntry, float> >& res,
                            const TEntry& base,
                            const TCandContainer& candidates,
                            const TEntryAccessPolicy& accessPolicy,
                            double minSim = 0,
                            size_t maxRet = (std::numeric_limits<size_t>::max)() )
   {
      res.clear();
      double simVal;

      typename TCandContainer::const_iterator cIt;
      for ( cIt = candidates.begin(); cIt != candidates.end(); ++cIt )
      {
         simVal = getCosOfAngle(base, *cIt, accessPolicy);
         if ( simVal > minSim )
            res.push_back( std::make_pair(*cIt, static_cast<float>(simVal)) );
      }

      // sort
      if ( maxRet < res.size() / 2 )
         partial_sort(res.begin(), res.begin() + maxRet, res.end(), CosSimilarity::comparer<TEntry>);
      else
         sort (res.begin(), res.end(), CosSimilarity::comparer<TEntry>);

      if ( maxRet < res.size() )
         res.resize( maxRet );
   }

   //////////////////////////////////////////////////////////////////////////
   template< typename TEntry, typename TEntryAccessPolicy>
   static double getNorm( const TEntry& entry, const TEntryAccessPolicy& accessPolicy )
   {
      return getNorm( accessPolicy.get_begin(entry), accessPolicy.get_end(entry), accessPolicy );
   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
private:

   template <typename TEntry>
   static bool comparer( std::pair<TEntry, float>& p1, std::pair<TEntry, float>& p2 )
   { return p1.second > p2.second; }

   //////////////////////////////////////////////////////////////////////////

   //template< typename TEntryIterator, typename TEntryAccessPolicy>
   //static double getNorm( TEntryIterator beg, TEntryIterator end, 
   //                       const TEntryAccessPolicy& accessPolicy )
   //{
   //   double norm = 0;
   //   for (; beg != end; ++beg)
   //      norm += accessPolicy.get_score(beg) * accessPolicy.get_score(beg);
   //   return std::sqrt(norm);
   //}

   //////////////////////////////////////////////////////////////////////////

   template< typename TEntry, typename TEntryAccessPolicy>
   static double getCosOfAngle( const TEntry& entryA, const TEntry& entryB,
                                const TEntryAccessPolicy& accessPolicy )
   {
      std::pair<double, int> simVal = getDotProduct(entryA, entryB, accessPolicy);
      simVal.first /= ( accessPolicy.get_norm(entryA) * accessPolicy.get_norm(entryB) );
      return accessPolicy.apply_post_process(entryA, entryB, simVal);
   }

   //////////////////////////////////////////////////////////////////////////

   template< typename TEntry, typename TEntryAccessPolicy>
   static std::pair<double, int>
      getDotProduct( const TEntry& entryA, const TEntry& entryB,
                     const TEntryAccessPolicy& accessPolicy )
   {
      return getDotProduct( accessPolicy.get_begin(entryA), accessPolicy.get_end(entryA), 
                            accessPolicy.get_begin(entryB), accessPolicy.get_end(entryB),
                            accessPolicy );
   }
   
   //////////////////////////////////////////////////////////////////////////

   template< typename TEntryIterator, typename TEntryAccessPolicy>
   static std::pair<double, int>
      getDotProduct( TEntryIterator begA, TEntryIterator endA,
                     TEntryIterator begB, TEntryIterator endB,
                     const TEntryAccessPolicy& accessPolicy )
   {
      std::pair<double, int> res(0,0);
      for (; begA != endA && begB != endB; )
      {
         if ( accessPolicy.get_id(begA) < accessPolicy.get_id(begB) )
            ++begA;
         else if ( accessPolicy.get_id(begB) < accessPolicy.get_id(begA) )
            ++begB;
         else
         {
            res.first += accessPolicy.get_score(begA) * accessPolicy.get_score(begB);
            ++(res.second);
            ++begA;
            ++begB;
         }
      }
      
      return res;
   }
};

}}

// -----------------------------------------------------------------------------

#endif // __COS_SIMILARITY_H

// -----------------------------------------------------------------------------

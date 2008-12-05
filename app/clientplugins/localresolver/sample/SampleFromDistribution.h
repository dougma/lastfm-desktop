// Sample from EMPIRICAL (given) distribution.
// There are three types of sampling here:
// linear: uses a simple linear search that does not guarantee
//         that the number of requested samples will be found.
//         Use it for LARGE distributions
// tree: Simple tree balanced on probability. The best solution for
//       small/medium distributions
// justrandom: simply pull random samples

#ifndef __SAMPLE_FROM_DISTRIBUTION_H
#define __SAMPLE_FROM_DISTRIBUTION_H

#include <algorithm>
#include <vector>
#include <set>
#include <deque>
#include <ctime>
#include <list>
#include <limits>

#include <boost/random.hpp>

namespace fm { namespace last { namespace algo {

// -----------------------------------------------------------------------------

struct DerefPolicy
{
   template <typename IT>
   typename std::iterator_traits<IT>::value_type& operator()(IT& el) const
   { return *el; }

   template <typename IT>
   const typename std::iterator_traits<const IT>::value_type& operator()(const IT& el) const
   { return *el; }
};

// -----------------------------------------------------------------------------

// copy the value of the iterator
struct CopyValuePolicy
{
   template <typename InIT, typename OutIT>
   void operator() ( const InIT& from, OutIT& to ) const
   {
      *to = *from;
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

// copy the iterator
struct CopyIteratorPolicy
{
   template <typename InIT, typename OutIT>
   void operator() ( const InIT& from, OutIT& to ) const
   {
      to = from;
   }

   template <typename InIT>
   const InIT operator() ( const InIT& it ) const
   {
      return it;
   }

   template <typename InIT>
   InIT operator() ( InIT& it ) const
   {
      return it;
   }
};

// -----------------------------------------------------------------------------

template < typename AccessPolicy = DerefPolicy,
           typename CopyPolicy = CopyValuePolicy>
class ListBasedSampler
{
   const CopyPolicy   m_copyElement;
   const AccessPolicy m_accessElement;

   boost::mt19937                            m_randomGenerator;
   // be very careful with this!! even if the methods are const
   // they are NOT thread safe due to this variable!
   mutable boost::uniform_01<boost::mt19937> m_uniform01Distr;

public:

   ListBasedSampler() :
      m_copyElement(),
      m_accessElement(),
      m_randomGenerator(),
      m_uniform01Distr(m_randomGenerator)
      {
         boost::uint32_t localSeed = time(0);
            //static_cast<boost::uint32_t>( reinterpret_cast<boost::int64_t>(this) %
            //(std::numeric_limits<boost::uint32_t>::max)() );

         m_uniform01Distr.base().seed(localSeed);
      }

   // Sample a group of elements (with NO repetition) from a given distribution
   // isPDF assumes that the passed data is a probability distribution
   // function, and therefore the sum of all it's elements is = 1
   template <typename InIT, typename OutIT>
   OutIT multiSample( InIT first, InIT last, OutIT dest,
                      int numToSample,
                      bool isPDF = false ) const
   {
      int sourceSize = 0;
      for ( InIT it = first; it != last; ++it )
         ++sourceSize;
      if ( sourceSize <= numToSample )
      {
         for ( ; first != last; ++first )
         {
            m_copyElement(first, dest);
            ++dest;
         }
         return dest;
      }

      std::list<InIT> elements;
      for ( InIT it = first; it != last; ++it )
         elements.push_back(it);

      double sum;

      if ( isPDF )
         sum = 1;
      else
      {
         sum = 0;
         for ( InIT it = first; it != last; ++it )
            sum += m_accessElement(it);
      }

      std::vector<double> randList;
      typename std::list<InIT>::iterator lIt;

      int inserted = 0;
      while ( inserted < numToSample && !elements.empty() )
      {
         randList.resize(numToSample - inserted);
         for (int i = 0; i < numToSample - inserted; ++i)
            randList[i] = m_uniform01Distr() * sum;
         std::sort(randList.begin(), randList.end(), std::greater<double>() );

         std::vector<double>::const_iterator randIt = randList.begin();
         double summedPos = sum;

         for ( lIt = elements.begin(); lIt != elements.end() && randIt != randList.end();  )
         {
            InIT& currIt = *lIt; // just for readability
            summedPos -= m_accessElement(currIt);

            if ( *randIt < summedPos )
            {
               ++lIt;
               continue;
            }

            m_copyElement( currIt, dest );
            ++dest;

            sum -= m_accessElement(currIt);
            elements.erase(lIt++);
            ++inserted;

            // the other random values
            ++randIt;
            for ( ; randIt != randList.end(); ++randIt )
            {
               if ( *randIt < summedPos )
                  break;
            }
         }
      }

      return dest;
   }

   // -----------------------------------------------------------------------------

   // sample just one element from the distribution
   // isPDF assumes that the passed data is a probability distribution
   // function, and therefore the sum of all it's elements is = 1
   template <typename IT>
   typename std::iterator_traits<IT>::value_type
      singleSample( IT first, IT last,
                    bool isPDF = false ) const
   {
      double sum;
      if ( isPDF )
         sum = 1;
      else
      {
         sum = 0;
         for ( IT it = first; it != last; ++it )
            sum += m_accessElement(it);
      }

      double randPos = m_uniform01Distr() * sum;
      double summedPos = sum;

      IT foundIt;
      for ( foundIt = first; foundIt != last ; ++foundIt )
      {
         summedPos -= m_accessElement(foundIt);
         if ( randPos > summedPos )
            break;
      }

      if ( foundIt == last )
         return m_copyElement(first);
      else
         return m_copyElement(foundIt);
   }

};



// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// MOSTLY DEPRECATED STUFF!!!
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------

// Sample a group of elements (with NO repetition) from a given distribution/
// The weight of each element is given by getValue which taken an iterator
// returns the weight.
// isPDF assumes that the passed data is a probability distribution
// function, and therefore the sum of all it's elements is = 1
template <typename InIT, typename OutIT, typename AccessPolicy> inline
OutIT linearSample( InIT first, InIT last, OutIT dest,
                    int maxNum, const AccessPolicy getValue,
                    boost::uniform_01<boost::mt19937>* pRandGenerator = NULL,
                    bool allowDups = false,
                    bool isPDF = false )
{
   //typedef typename boost::iterator_value<IT>::type t_valType;
   //typedef BOOST_TYPEOF(getValue(first)) t_type;

   double sum;

   if ( isPDF )
      sum = 1;
   else
   {
      sum = 0;
      for ( InIT it = first; it != last; ++it )
         sum += getValue(it);
   }


   bool localRng = false;
   if ( !pRandGenerator )
   {
      boost::mt19937 rng;
      time_t ltime;
      time(&ltime);
      rng.seed(static_cast<int>(ltime));
      pRandGenerator = new boost::uniform_01<boost::mt19937>(rng); // passed by value
      localRng = true;
   }

   std::vector<double> randList(maxNum);
   for (int i = 0; i < maxNum; ++i)
      randList[i] = (*pRandGenerator)() * sum;

   std::sort(randList.begin(), randList.end(), std::greater<double>() );

   double summedPos = sum;
   std::set<InIT> foundSet;
   std::vector<double>::const_iterator randIt = randList.begin();

   for ( InIT it = first; it != last && randIt != randList.end(); ++it )
   {
      summedPos -= getValue(it);

      if ( *randIt < summedPos )
         continue;

      if ( allowDups )
         *dest++ = *it;
      else
         foundSet.insert(it);

      // TEST THAT!!!
      // the other random values
      ++randIt;
      for ( ; randIt != randList.end(); ++randIt )
      {
         if ( *randIt < summedPos )
            break;
         if ( allowDups )
            *dest++ = *it;
      }
   }

   if ( localRng )
      delete pRandGenerator;

   if ( allowDups )
      return dest;
   else
   {
      typename std::set<InIT>::const_iterator sIt;
      OutIT toFillIt = dest;
      for ( sIt = foundSet.begin(); sIt != foundSet.end(); ++sIt )
         *toFillIt++ = *(*sIt);

      return toFillIt;
   }
}

// -----------------------------------------------------------------------------

template <typename InIT, typename OutIT> inline
OutIT linearSample( InIT first, InIT last, OutIT dest,
                    int maxNum,
                    boost::uniform_01<boost::mt19937>* pRandGenerator = NULL,
                    bool allowDups = false, bool isPDF = false )
{
   return linearSample(first, last, dest, maxNum, DerefPolicy(), pRandGenerator, allowDups, isPDF );
}

// -----------------------------------------------------------------------------

// sample just one element from the distribution
// isPDF assumes that the passed data is a probability distribution
// function, and therefore the sum of all it's elements is = 1
template <typename T, typename InIT, typename AccessPolicy> inline
T& linearSample( InIT first, InIT last,
                 const AccessPolicy getValue,
                 boost::uniform_01<boost::mt19937>* pRandGenerator = NULL,
                 bool isPDF = false )
{
   double sum;
   if ( isPDF )
      sum = 1;
   else
   {
      sum = 0;
      for ( InIT it = first; it != last; ++it )
         sum += getValue(it);
   }

   bool localRng = false;
   if ( !pRandGenerator )
   {
      boost::mt19937 rng;
      time_t ltime;
      time(&ltime);
      rng.seed(static_cast<int>(ltime));
      pRandGenerator = new boost::uniform_01<boost::mt19937>(rng); // passed by value
      localRng = true;
   }

   double randPos = (*pRandGenerator)() * sum;
   double summedPos = sum;

   InIT foundIt;
   for ( foundIt = first; foundIt != last ; ++foundIt )
   {
      summedPos -= getValue(foundIt);
      if ( randPos > summedPos )
         break;
   }

   if ( localRng )
      delete pRandGenerator;

   if ( foundIt == last )
      return *first;
   else
      return *foundIt;
}

// -----------------------------------------------------------------------------

template <typename T, typename InIT> inline
T& linearSample( InIT first, InIT last,
                 boost::uniform_01<boost::mt19937>* pRandGenerator = NULL,
                 bool isPDF = false )
{
   return linearSample<T>(first, last, DerefPolicy(), pRandGenerator, isPDF );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// Sample a group of elements (with NO repetition) from a given distribution
// The weight of each element is given by getValue which taken an iterator
// returns the weight.
// isPDF assumes that the passed data is a probability distribution
// function, and therefore the sum of all it's elements is = 1
template <typename InIT, typename OutIT, typename AccessPolicy > inline
OutIT listLinearSample( InIT first, InIT last, OutIT dest,
                        int maxNum, AccessPolicy getValue,
                        boost::uniform_01<boost::mt19937>* pRandGenerator = NULL,
                        bool isPDF = false)
{
   if ( last - first <= maxNum )
   {
      copy(first, last, dest);
      return dest;
   }

   std::list<InIT> elements;
   for ( InIT it = first; it != last; ++it )
      elements.push_back(it);

   double sum;

   if ( isPDF )
      sum = 1;
   else
   {
      sum = 0;
      for ( InIT it = first; it != last; ++it )
         sum += getValue(it);
   }

   bool localRng = false;
   if ( !pRandGenerator )
   {
      boost::mt19937 rng;
      time_t ltime;
      time(&ltime);
      rng.seed(static_cast<int>(ltime));
      pRandGenerator = new boost::uniform_01<boost::mt19937>(rng); // passed by value
      localRng = true;
   }

   //boost::uniform_real<> distr(0.0, 1.0);
   //boost::variate_generator<boost::mt19937&, boost::uniform_real<> >
   //   rand_generator(rng, distr);

   std::vector<double> randList;
   typename std::list<InIT>::iterator lIt;

   int inserted = 0;
   while ( inserted < maxNum && !elements.empty() )
   {
      randList.resize(maxNum - inserted);
      for (int i = 0; i < maxNum - inserted; ++i)
         randList[i] = (*pRandGenerator)() * sum;
      std::sort(randList.begin(), randList.end(), std::greater<double>() );

      std::vector<double>::const_iterator randIt = randList.begin();
      double summedPos = sum;

      for ( lIt = elements.begin(); lIt != elements.end() && randIt != randList.end();  )
      {
         InIT& currIt = *lIt; // just for readability
         summedPos -= getValue(currIt);

         if ( *randIt < summedPos )
         {
            ++lIt;
            continue;
         }

         *dest++ = *currIt;

         sum -= getValue(currIt);
         elements.erase(lIt++);
         ++inserted;

         // the other random values
         ++randIt;
         for ( ; randIt != randList.end(); ++randIt )
         {
            if ( *randIt < summedPos )
               break;
         }
      }
   }

   if ( localRng )
      delete pRandGenerator;

   return dest;
}

// -----------------------------------------------------------------------------

template <typename InIT, typename OutIT> inline
OutIT listLinearSample( InIT first, InIT last, OutIT dest,
                        int maxNum,
                        boost::uniform_01<boost::mt19937>* pRandGenerator = NULL,
                        bool isPDF = false )
{
   return listLinearSample(first, last, dest, maxNum, DerefPolicy(), pRandGenerator, isPDF );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

template <typename IT, typename AccessPolicy>
class TreeSampler
{

public:

   TreeSampler( IT first, IT last)
      : m_maxVal(-1)
   {
      m_pHead = new Node();
      buildTree(m_pHead, first, last);
   }

   ~TreeSampler()
   {
      destroyTree(m_pHead);
      //delete m_pHead;
   }

   // important: this assumes that the distribution passed to the
   // constructor had MORE elements than the requested in howMany
   template <typename OutIT>
   void sample(OutIT dest, int howMany);

private:

   struct Node
   {
      Node()
         :  m_doNotVisit(false), m_isLeaf(false), m_pLeft(NULL), m_pRight(NULL) {}

      // if all three elements (local, left and right)
      // have already been selected, then this node
      // and all its children should not be visited
      bool m_doNotVisit;

      // if the value is below the limit, go left,
      // otherwise go right
      double m_limit;

      Node* m_pLeft;
      Node* m_pRight;

      bool m_isLeaf;
      IT   m_leafVal;

   };

   AccessPolicy  m_getValueFun;
   Node* m_pHead;
   double m_maxVal;

   void destroyTree( Node* pHead );
   void buildTree( Node* pHead, IT first, IT last, double prevLimit = 0 );
   bool find( Node* pHead, double val, IT& foundIT, bool removeFound = false );
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

template <typename IT, typename AccessPolicy>
void TreeSampler<IT, AccessPolicy>::destroyTree( Node* pHead )
{
   if ( pHead->m_pLeft->m_isLeaf )
   {
      delete pHead->m_pLeft;
      pHead->m_pLeft = NULL;
   }
   else
      destroyTree(pHead->m_pLeft);

   if ( pHead->m_pRight->m_isLeaf )
   {
      delete pHead->m_pRight;
      pHead->m_pRight = NULL;
   }
   else
      destroyTree(pHead->m_pRight);

   delete pHead;
}

// -----------------------------------------------------------------------------

template <typename IT, typename AccessPolicy>
void TreeSampler<IT, AccessPolicy>::buildTree( Node* pHead, IT first, IT last, double prevLimit /*= 0 */ )
{
   if ( last-first <= 1 )
   {
      pHead->m_isLeaf = true;
      pHead->m_leafVal = first;
      return;
   }

   double sum = 0;
   IT it;
   for ( it = first; it != last; ++it )
      sum += m_getValueFun(it);

   if ( m_maxVal < 0 )
      m_maxVal = sum;

   double th = sum/2.0 + 0.00001;

   sum = 0;
   for ( it = first; it != last; ++it )
   {
      sum += m_getValueFun(it);
      if ( sum > th )
         break;
   }

   pHead->m_pLeft = new Node();
   pHead->m_pRight = new Node();

   if ( it == first )
   {
      pHead->m_limit = m_getValueFun(first) + prevLimit;
      ++it;
   }
   else
      pHead->m_limit = sum - m_getValueFun(it) + prevLimit;

   buildTree( pHead->m_pLeft, first, it, prevLimit );
   buildTree( pHead->m_pRight, it, last, pHead->m_limit );
}

// -----------------------------------------------------------------------------

template <typename IT, typename AccessPolicy>
bool TreeSampler<IT, AccessPolicy>::find( Node* pHead, double val, IT& foundIT, bool removeFound /*= false */ )
{
   if ( pHead->m_doNotVisit )
      return false;

   if ( pHead->m_isLeaf )
   {
      foundIT = pHead->m_leafVal;
      if ( removeFound )
         pHead->m_doNotVisit = true;
      return true;
   }

   if ( removeFound && pHead->m_pLeft->m_doNotVisit && pHead->m_pRight->m_doNotVisit )
   {
      pHead->m_doNotVisit = true;
      return false;
   }

   if ( val <= pHead->m_limit)
      return find(pHead->m_pLeft, val, foundIT, removeFound);
   else
      return find(pHead->m_pRight, val, foundIT, removeFound);
}

// -----------------------------------------------------------------------------

template <typename IT, typename AccessPolicy>
template <typename OutIT>
void TreeSampler<IT, AccessPolicy>::sample( OutIT dest, int howMany )
{
   int numInserted = 0;
   // generate a random distribution;
   boost::mt19937 rng;

#ifdef NDEBUG
   time_t ltime;
   time(&ltime);
   rng.seed(static_cast<int>(ltime));
#endif

   boost::uniform_real<> distr(0.0, m_maxVal);
   boost::variate_generator<boost::mt19937&, boost::uniform_real<> >
      rand_generator(rng, distr);

   IT tmpIt;
   double tmpVal;

   while ( numInserted < howMany && !m_pHead->m_doNotVisit )
   {
      tmpVal = rand_generator();
      if ( !find(m_pHead, tmpVal, tmpIt, true) )
         continue; // none found here!

      *dest++ = *tmpIt;
      ++numInserted;

   }

}

// -----------------------------------------------------------------------------

// helper function
template <typename InIT, typename OutIT, typename AccessPolicy> inline
void treeSample( InIT first, InIT last, OutIT dest,
                 int howMany, AccessPolicy getValue, bool checkSmallSet = true )
{
   // not enough content for sampling!
   // Just copy what was already in there
   if ( checkSmallSet && (last - first) <= howMany )
   {
      for ( ; first != last; ++first, ++dest )
         *dest = *first;
      return;
   }

   TreeSampler<InIT, AccessPolicy> ts(first, last);
   ts.sample(dest, howMany);
}

// -----------------------------------------------------------------------------

template <typename InIT, typename OutIT> inline
void treeSample( InIT first, InIT last, OutIT dest, int howMany )
{
   treeSample(first, last, dest, howMany, DerefPolicy() );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

template <typename InIT, typename OutIT> inline
void randomSample( InIT first, InIT last, OutIT dest, int howMany, bool checkSmallSet = true )
{
   int numEl = last-first;

   if ( checkSmallSet && numEl <= howMany )
   {
      for ( ; first != last; ++first, ++dest )
         *dest = *first;
      return;
   }


   // generate a random distribution;
   boost::mt19937 rng;

#ifdef NDEBUG
   time_t ltime;
   time(&ltime);
   rng.seed(static_cast<int>(ltime));
#endif
   boost::uniform_int<> distr(0, numEl-1);
   boost::variate_generator<boost::mt19937&, boost::uniform_int<> >
      rand_generator(rng, distr);

   // chosing different strategies depending on the size of the
   // passed distribution

   double ratio = static_cast<double>(howMany) / static_cast<double>(numEl);

   if ( ratio > 0.5 )
   {
      std::vector<char> randList(numEl, 0);
      int found = 0, pos;
      while ( found < howMany )
      {
         pos = rand_generator();
         if ( randList[pos] == 0 )
         {
            randList[pos] = 1;
            ++found;
         }
      }

      std::vector<char>::iterator it;
      for (it = randList.begin(); it != randList.end(); ++it, ++first)
      {
         if ( *it != 0 )
            *dest++ = *first;
      }
   }
   else
   {
      std::set<int> randList;
      do
      {
         randList.insert(rand_generator());
      } while( static_cast<int>(randList.size()) < howMany );

      std::set<int>::iterator it;
      for (it = randList.begin(); it != randList.end(); ++it)
      {
         *dest++ = first[*it];
      }
   }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// samples according to a distribution
template <typename TKey, typename TValue>
class VecProdSampler
{
public:
   VecProdSampler(size_t sample_size, boost::uint32_t seedVal = 10)
      : m_sample_size(sample_size), m_rand(boost::mt19937(), boost::uniform_real<float>())
   {
      m_rand.engine().seed(seedVal);
   }

   void add(TKey key, TValue value);
   TKey get();
   bool get(TKey& key);

   size_t size()        const { return m_points.size(); }
   size_t sample_size() const { return m_sample_size; }

private:

   typename std::deque< std::pair < TKey, float> >::iterator m_it;

   std::deque< std::pair < TKey, float> > m_points;
   size_t                                 m_sample_size;
   boost::variate_generator< boost::mt19937, boost::uniform_real<float> > m_rand;
};

// -----------------------------------------------------------------------------

template <typename TKey, typename TValue>
void VecProdSampler<TKey, TValue>::add(TKey key, TValue value)
{
   if (value == 0) // we don\'t need no null points
      return;

   std::pair < TKey, float> item(key, pow(static_cast<float>(value), 0.1F) * m_rand());

   typename std::deque< std::pair < TKey, float> >::iterator it =
      lower_bound(m_points.begin(), m_points.end(), item,
                  utils::pair_traits::which<2>::comparer<std::greater>() );

   m_points.insert(it, item);

   if (m_points.size() > m_sample_size)
      m_points.pop_back();

   m_it = m_points.begin();
}

// -----------------------------------------------------------------------------

template <typename TKey, typename TValue>
bool VecProdSampler<TKey, TValue>::get(TKey& key)
{
   if ( m_it == m_points.end() )
      return false;

   key = m_it->first;
   ++m_it;

   return true;
}

// -----------------------------------------------------------------------------

template <typename TKey, typename TValue>
TKey VecProdSampler<TKey, TValue>::get()
{
   if ( m_it == m_points.end() )
      throw std::runtime_error("End of the distribution!");

   TKey key = m_it->first;
   ++m_it;
   return key;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

template <typename InIT, typename OutIT, typename AccessPolicy> inline
void vecprodSample( InIT first, InIT last, OutIT dest,
                    int howMany, const AccessPolicy getValue, bool checkSmallSet = true )
{
   size_t numEl = last-first;
   if ( checkSmallSet && static_cast<int>(numEl) <= howMany )
   {
      for ( ; first != last; ++first, ++dest )
         *dest = *first;
      return;
   }

#ifdef NDEBUG
   time_t ltime;
   time(&ltime);
   VecProdSampler<InIT, double> sampler(howMany, static_cast<boost::uint32_t>(ltime) );
#else
   VecProdSampler<InIT, double> sampler(howMany);
#endif

   // preparing the structure
   for (; first != last; ++first )
      sampler.add(first, getValue(first));

   // now get the data
   for ( int i = 0; i < howMany; ++i, ++dest )
      *dest = *(sampler.get());

}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

}}} // end of namespaces

#endif // __SAMPLE_FROM_DISTRIBUTION_H


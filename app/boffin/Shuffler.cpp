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


////////////////////////////////////////////////////////////////////////


// lifted from playdar
int levenshtein(const QString& source, const QString& target) 
{
  // Step 1
  const int n = source.length();
  const int m = target.length();
  if (n == 0) {
    return m;
  }
  if (m == 0) {
    return n;
  }
  // Good form to declare a TYPEDEF
  typedef std::vector< std::vector<int> > Tmatrix; 
  Tmatrix matrix(n+1);
  // Size the vectors in the 2.nd dimension. Unfortunately C++ doesn't
  // allow for allocation on declaration of 2.nd dimension of vec of vec
  for (int i = 0; i <= n; i++) {
    matrix[i].resize(m+1);
  }
  // Step 2
  for (int i = 0; i <= n; i++) {
    matrix[i][0]=i;
  }
  for (int j = 0; j <= m; j++) {
    matrix[0][j]=j;
  }
  // Step 3
  for (int i = 1; i <= n; i++) {
    const QChar s_i = source[i-1];
    // Step 4
    for (int j = 1; j <= m; j++) {
      const QChar t_j = target[j-1];
      // Step 5
      int cost;
      if (s_i == t_j) {
        cost = 0;
      }
      else {
        cost = 1;
      }
      // Step 6
      const int above = matrix[i-1][j];
      const int left = matrix[i][j-1];
      const int diag = matrix[i-1][j-1];
      //int cell = min( above + 1, min(left + 1, diag + cost));
      int cell = (((left+1)>(diag+cost))?diag+cost:left+1);
      if(above+1 < cell) cell = above+1;
      // Step 6A: Cover transposition, in addition to deletion,
      // insertion and substitution. This step is taken from:
      // Berghel, Hal ; Roach, David : "An Extension of Ukkonen's 
      // Enhanced Dynamic Programming ASM Algorithm"
      // (http://www.acm.org/~hlb/publications/asm/asm.html)
      if (i>2 && j>2) {
        int trans=matrix[i-2][j-2]+1;
        if (source[i-2]!=t_j) trans++;
        if (s_i!=target[j-2]) trans++;
        if (cell>trans) cell=trans;
      }
      matrix[i][j]=cell;
    }
  }
  // Step 7
  return matrix[n][m];
}


float
normalisedLevenshtein(const BoffinPlayableItem& a, const BoffinPlayableItem& b)
{
    // logic lifted from playdar's Resolver::calculate_score
    // not yet comparing album titles

    using namespace boost;
    // original names from the query:
    QString o_art = a.artist().simplified().toLower();
    QString o_trk = a.track().simplified().toLower();

    // names from candidate result:
    QString art = b.artist().simplified().toLower();
    QString trk = b.track().simplified().toLower();

    // short-circuit for exact match
    if (o_art == art && o_trk == trk) return 1.0;

    // the real deal, with edit distances:
    int trked = levenshtein(trk, o_trk);
    int arted = levenshtein(art, o_art);

    // tolerances:
    const float tol_art = 1.5;
    const float tol_trk = 1.5;
    
    // names less than this many chars aren't dismissed based on % edit-dist:
    const int grace_len = 6; 
    
    // if % edit distance is greater than tolerance, fail them outright:
    if( o_art.length() > grace_len && arted > o_art.length()/tol_art )
        return 0.0;

    if( o_trk.length() > grace_len && trked > o_trk.length()/tol_trk )
        return 0.0;

    // if edit distance longer than original name, fail them outright:
    if( arted >= o_art.length() )
        return 0.0;

    if( trked >= o_trk.length() )
        return 0.0;
    
    // combine the edit distance of artist & track into a final score:
    float artdist_pc = (o_art.length()-arted) / (float) o_art.length();
    float trkdist_pc = (o_trk.length()-trked) / (float) o_trk.length();
    return artdist_pc * trkdist_pc;
}


////////////////////////////////////////////////////////////////////////


Shuffler::Shuffler(QObject* parent /* = 0 */)
: QObject(parent)
, m_artistHistorySize(4)        // to mix up the artists
, m_songHistorySize(100)        // to suppress dup songs
{
}

BoffinPlayableItem 
Shuffler::sampleOne()
{
    BoffinPlayableItem result = sample();
    if (result.isValid()) {
        // artist memory
        m_artistHistory.push_back(result.artist());
        while (m_artistHistory.size() > m_artistHistorySize) {
            m_artistHistory.pop_front();
        }
        // track memory
        m_songHistory.push_back(result);
        while (m_songHistory.size() > m_songHistorySize) {
            m_songHistory.pop_front();
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

void
Shuffler::clearHistory()
{
    m_artistHistory.clear();
    m_songHistory.clear();
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
    return pushdownSong(item) * 
        (m_artistHistory.contains(item.artist(), Qt::CaseInsensitive) ? 0.00001 : 1.0);
}


float
Shuffler::pushdownSong(const BoffinPlayableItem& item)
{
    int i = 1;
    float result = 1.0;
    foreach(const BoffinPlayableItem& historicItem, m_songHistory) {
        float nl = normalisedLevenshtein(item,  historicItem);
        // levenshtein values not very reliable when less than 0.5
        if (nl > 0.5) {
            float score = 0.1 * (nl * i / (float) m_songHistorySize);
            if (score < result) {
                result = score;
            }
        }
        i++;
    }
    return result;
}

void 
Shuffler::receivePlayableItem(BoffinPlayableItem item)
{
    m_items.push_back(item);
}
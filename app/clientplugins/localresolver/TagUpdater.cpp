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

#include "TagUpdater.h"
#include "LocalCollection.h"
#include "lib/lastfm/ws/WsReply.h"
#include "lib/lastfm/types/Artist.h"
#include "lib/lastfm/types/Tag.h"
#include <QDateTime>
#include <QTimer>

#define TAGUPDATER_ARTIST_TAG_LIMIT 10          
#define TAGUPDATER_LOWEST_ARTIST_TAG_WEIGHT 10  /* out of 100 */
#define TAGUPDATER_IDLE_WAIT_MS (15*60*1000)


TagUpdater::TagUpdater()
: m_collection(0)
, m_timer(0)
{
}

TagUpdater::~TagUpdater()
{
    exit();
    wait();
}


void
TagUpdater::run()
{
    m_collection = LocalCollection::create("TagUpdaterConnection");
    m_timer = new QTimer();
    m_timer->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), SLOT(launchNextBatch()));

    launchNextBatch();
    exec();

    // kill the timer and active requests
    m_timer->stop();
    foreach (WsReply* r, m_activeRequests) {
        disconnect(r, SIGNAL(finished(WsReply*)), this, SLOT(onWsFinished(WsReply*)));
        r->abort();
    }
    m_activeRequests.clear();
    delete m_timer;
    delete m_collection;
}

void
TagUpdater::launchNextBatch()
{
    if (0 == startRequests(m_collection->artistsWithExpiredTags())) {
        if (0 == startRequests(m_collection->artistsNeedingTagUpdate())) {
            m_timer->start(TAGUPDATER_IDLE_WAIT_MS);
        }
    }
}

// returns the number of requests started
int
TagUpdater::startRequests(QStringList artists)
{
    int count = 0;
    foreach (const QString& artist, artists) {
        WsReply* r = Artist(artist).getTopTags();
        r->setAssociatedData(artist);
        bool connected = connect(
            r, SIGNAL(finished(WsReply*)), 
            this, SLOT(onWsFinished(WsReply*)), 
            Qt::DirectConnection);      // AutoConnection would queue it to the main thread (not what we want here)
        Q_ASSERT(connected);
        m_activeRequests += r;
        count++;
    }

    return count;
}

void 
TagUpdater::onWsFinished(WsReply* r)
{
    QDateTime now, next;
    QString artist(r->associatedData().toString());
    Q_ASSERT(!artist.isEmpty());

    try {
        Ws::Error err = r->error();
        // Ws::InvalidParameters occurs when the artist doesn't exist, and
        // for our purposes that's not an error
        if (err == Ws::NoError  ||  
            err == Ws::InvalidParameters) 
        {
            // limit the number of tags:
            // no tags with weight lower than TAGUPDATER_LOWEST_ARTIST_TAG_WEIGHT
            // and no more than TAGUPDATER_ARTIST_TAG_LIMIT number of tags
            WeightedStringList wsl(Tag::list(r));
            wsl.weightedSort(Qt::DescendingOrder);
            WeightedStringList::iterator cutoff = wsl.begin();
            while ( cutoff < wsl.end() && 
                (cutoff - wsl.begin()) < TAGUPDATER_ARTIST_TAG_LIMIT  &&
                cutoff->weighting() > TAGUPDATER_LOWEST_ARTIST_TAG_WEIGHT)
            {
                cutoff++;
            }
            wsl.erase(cutoff, wsl.end());

            // set the tags in the db
            m_collection->setGlobalTagsForArtist(artist, wsl);

            // that worked, set times for updateArtistDownload call
            now = QDateTime::currentDateTime();
            next = r->expires();
            if (!next.isValid() || next < now || next > now.addYears(1)) {
                next = now.addYears(1);
            }
        } else {
            warning("error: " + err);
        }
    } 
    catch (QSqlError& e) {
        exception(e.text());
    }
    catch (...) {
        exception("unexpected");
    }
    
    try {
        // an exception now is fatal to the TagUpdater
        if (!next.isValid()) {
            // this would mean a failed request, or 
            // failed handling of the response. So,
            // bump the next_dl_time to effectively 
            // delay before trying again:
            // todo: should probably delay for increasing periods
            next = QDateTime::currentDateTime().addSecs(300);
        }
        // 'now' may be invalid, which is fine: it 
        // means we didn't update the artist details
        m_collection->updateArtistDownload(artist, next, now);

        m_activeRequests -= r;
        if (0 == m_activeRequests.size()) {
            launchNextBatch();
        }
    } 
    catch (QSqlError& e) {
        fatal(e.text());
    }
    catch (...) {
        fatal("unexpected");
    }
}

void
TagUpdater::warning(const QString& msg)
{
    //todo: log
    int ii = 0;
}

void
TagUpdater::exception(const QString& msg)
{
    //todo: log
    int ii = 0;
}

void
TagUpdater::fatal(const QString& msg)
{
    //todo: log
    quit();
}


/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
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

#include "TrackTagUpdater.h"

#include <QThread>
#include <QTimer>
#include "LocalCollection.h"
#include "TagifierRequest.h"
#include "QueryError.h"


//static
TrackTagUpdater*
TrackTagUpdater::create(const QString& webServiceUrl, unsigned tagValidityDays, unsigned interRequestDelayMins)
{
    QThread* thread = new QThread();
    TrackTagUpdater* updater = new TrackTagUpdater(webServiceUrl, tagValidityDays, interRequestDelayMins);
    updater->moveToThread(thread);
    updater->setParent(thread);
    thread->start();
    return updater;
}


TrackTagUpdater::TrackTagUpdater(const QString& webServiceUrl, unsigned tagValidityDays, unsigned interRequestDelayMins)
: m_webServiceUrl(webServiceUrl)
, m_tagValidityDays(tagValidityDays)
, m_interRequestDelayMins(interRequestDelayMins)
, m_timer(0)
, m_collection(0)
, m_needsUpdate(false)
{
}

void
TrackTagUpdater::needsUpdate()
{
    Q_ASSERT(sender());     // must invoke via signal/slot connection for thread safety.

    m_needsUpdate = true;
    if (!m_timer) {
        m_timer = new QTimer();
        connect(m_timer, SIGNAL(timeout()), SLOT(doUpdateTags()));
        startTimer(1);
    }
}

void
TrackTagUpdater::startTimer(int seconds)
{
    m_timer->setSingleShot(true);
    m_timer->setInterval(seconds * 1000);
}

unsigned
TrackTagUpdater::secondsToNextUpdate()
{
    if (!m_lastRequestTimeUtc.isValid())
        return 0;

    int result = m_interRequestDelayMins * 60 - secondsSinceLastRequest();
    return result < 0 ? 0 : result;
}

int
TrackTagUpdater::secondsSinceLastRequest()
{
    QDateTime nowUtc = QDateTime::currentDateTime().toUTC();
    return nowUtc.toTime_t() - m_lastRequestTimeUtc.toTime_t();
}


void
TrackTagUpdater::doUpdateTags()
{
    if (m_needsUpdate) {
        try {
            m_needsUpdate = false;
            if (!m_collection)
                m_collection = LocalCollection::create("TrackTagUpdater");

            m_lastRequestTimeUtc = QDateTime::currentDateTime().toUTC();

            TagifierRequest* req = new TagifierRequest(m_collection, m_webServiceUrl);
            connect(req, SIGNAL(finished( int, int, int )), SLOT(onFinished( int, int, int )));
            if (req->makeRequest(m_tagValidityDays))
                return;     // onFinished will be signalled. bail now.

            delete req;
        } 
        catch(QueryError& e) {
            qWarning() << "TrackTagUpdater::doUpdateTags: " + e.text();
	    }
        catch(...) {
            qWarning() << "TrackTagUpdater::doUpdateTags: unhandled exception";
        }
    }

    startTimer(secondsToNextUpdate());
}

void 
TrackTagUpdater::onFinished(int requestIdCount, int responseIdCount, int responseTagCount)
{
    delete sender();
    int timeTaken = secondsSinceLastRequest();
    emit tagsUpdated(requestIdCount, responseIdCount, responseTagCount, timeTaken);
    startTimer(secondsToNextUpdate());
}
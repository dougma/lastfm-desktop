/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include "ResolvingTrackSource.h"
#include "Resolver.h"
#include <QTimer>


ResolvingTrackSource::ResolvingTrackSource(Resolver* resolver, AbstractTrackSource *src)
: m_resolver(resolver)
, m_src(src)
, m_firstDelay(true)
, m_waiting(true)
, m_started(false)
// user settable ones:
, m_minQueue(0)
, m_maxQueue(5)
, m_initialResolveDelay(1000)
, m_interTrackResolveDelay(250)
{
    Q_ASSERT(resolver && src);
    connect(src, SIGNAL(trackAvailable()), this, SLOT(onTrackAvailable()));
    connect(resolver, SIGNAL(resolveComplete(const Track)), this, SLOT(onResolveComplete(const Track)));
}

ResolvingTrackSource::~ResolvingTrackSource()
{
    delete m_src;
}

void
ResolvingTrackSource::start()
{
    fill();
}

void 
ResolvingTrackSource::setMaxQueueSize(unsigned maxQueue)
{
    m_maxQueue = maxQueue;
}

void 
ResolvingTrackSource::setMinQueueSize(unsigned minQueue)
{
    m_minQueue = minQueue;
}

void
ResolvingTrackSource::setInitialResolveDelay(int d)
{
    m_initialResolveDelay = d;
}

void
ResolvingTrackSource::setInterTrackResolveDelay(int d)
{
    m_interTrackResolveDelay = d;
}

void
ResolvingTrackSource::onTrackAvailable()
{
    if (m_waiting) {
        m_waiting = false;
        fill();
    }
}

void
ResolvingTrackSource::fill()
{
    while (0 == m_maxQueue || (unsigned) m_queue.size() < m_maxQueue) {
        Track t = m_src->takeNextTrack();
        if (t.isNull()) {
            m_waiting = true;
            break;
        }
        m_queue << t;
        m_resolver->resolve(t);
        startResolveTimer(t);
    }
}

void
ResolvingTrackSource::startResolveTimer(Track t)
{
    if (m_delaying.isNull()) {
        m_delaying = t;
        int delay = m_firstDelay 
            ? (m_firstDelay = false, m_initialResolveDelay) 
            : m_interTrackResolveDelay;
        QTimer::singleShot(delay, this, SLOT(onResolveTimeout()));
    }
}


void
ResolvingTrackSource::onResolveTimeout()
{
    if (!m_delaying.isNull()) {
        m_resolver->stopResolving(m_delaying);
        m_delaying = Track();
        emit trackAvailable();
    }
}

void
ResolvingTrackSource::onResolveComplete(const Track t)
{
    if (m_delaying == t) {
        m_delaying = Track();
        emit trackAvailable();
    }
}

Track 
ResolvingTrackSource::takeNextTrack()
{
    if (m_queue.isEmpty() || !m_delaying.isNull()) {
        return Track();
    }
    
    Track result;
    if (m_resolver->stillResolving(m_queue.front())) {
        startResolveTimer(m_queue.front());
    } else {
        result = m_queue.takeFirst();

        if ((unsigned) m_queue.size() <= m_minQueue) {
            fill();
        }
    }

    return result;
}

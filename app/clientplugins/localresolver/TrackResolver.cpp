/**************************************************************************
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

#include "TrackResolver.h"
#include "LocalContentScanner.h"
#include "LocalCollection.h"
#include "QueryError.h"

extern QString remapVolumeName(const QString& volume);


TrackResolver::TrackResolver()
: m_query(0)
, m_scanner(0)
{}

TrackResolver::~TrackResolver()
{
    delete m_scanner;
    delete m_query;
}

void
TrackResolver::init()
{
    m_query = QueryThread::create();
    m_scanner = new LocalContentScanner;
}

void 
TrackResolver::resolve(class ITrackResolveRequest *req)
{
    Q_ASSERT(req && m_query);
    if (req) {
        m_query->enqueue(req);
    }
}

void 
TrackResolver::finished()
{
    delete this;
}


//////////////////////////////////////////////////////////////////////

    
Response::Response(const LocalCollection::ResolveResult &r)
:   m_matchQuality( r.m_matchQuality ),
    m_artist( r.m_artist.toUtf8() ),
    m_album( r.m_album.toUtf8() ),
    m_title( r.m_title.toUtf8() ),
    m_duration( r.m_duration ),
    m_kbps( r.m_kbps )
{
    // create a nice file:// url, remap the volume name along the way...
    m_url = QUrl::fromLocalFile(remapVolumeName(r.m_sourcename) + r.m_path + r.m_filename).toEncoded();
}

float
Response::matchQuality() const
{
    return m_matchQuality;
}

const char *
Response::url() const
{
    return m_url.constData();
}

const char *
Response::artist() const
{
    return m_artist.constData();
}

const char *
Response::album() const
{
    return m_album.constData();
}

const char *
Response::title() const
{
    return m_title.constData();
}

const char *
Response::filetype() const
{
    return "audio/mpeg";    // yes this is mp3, todo: the others
}

unsigned 
Response::duration() const
{
    return m_duration;
}

unsigned 
Response::kbps() const
{
    return m_kbps;
}

void 
Response::finished()
{
    delete this;
}


//////////////////////////////////////////////////////////////////////

QueryThread*
QueryThread::create()
{
    QueryThread* a = new QueryThread;
    a->start();
    return a;
}

QueryThread::QueryThread()
:m_stopping(false)
{
}

QueryThread::~QueryThread()
{
    m_stopping = true;
    m_mutex.lock();
    m_wakeUp.wakeAll();
    m_mutex.unlock();
    wait();
}

void 
QueryThread::run()
{
    try {
        LocalCollection *pCollection = LocalCollection::create("TrackResolverConnection");

        while (!m_stopping) {
            m_mutex.lock();
            m_wakeUp.wait(&m_mutex);
            m_mutex.unlock();

            for(;;) {
                ITrackResolveRequest *req = 0;
                m_mutex.lock();
                if (!m_queue.isEmpty()) 
                    req = m_queue.takeFirst();
                m_mutex.unlock();
                    
                if (req == 0 || m_stopping) break;

                doRequest(pCollection, req);
            }
        }

        // finish any remaining requests without actually doing anything:
        while (!m_queue.isEmpty()) {
            m_queue.takeFirst()->finished();
        }

        delete pCollection;
    } 
    catch (QueryError &e) {
        qCritical() << "QueryThread::run: " + e.text();
    }
    catch (...) {
        qCritical() << "QueryThread::run: unhandled exception";
    }
}


void
QueryThread::doRequest(LocalCollection *pCollection, ITrackResolveRequest* req)
{
	try 
	{
        QTime start(QTime::currentTime());
        QList<LocalCollection::ResolveResult> results = 
            pCollection->resolve(
                QString::fromUtf8(req->artist()),
                QString::fromUtf8(req->album()),
                QString::fromUtf8(req->title()));

        foreach (const LocalCollection::ResolveResult &r, results) {
            QString filename(r.m_sourcename + r.m_path + r.m_filename);
            if (true /*bLooksReadable*/)
                req->result(new Response(r));
        }
        qDebug() << "resolve complete: " << start.msecsTo(QTime::currentTime()) << "ms";
	} 
	catch(QueryError &e) {
        qWarning() << "QueryThread::doRequest: " + e.text();
	}
    catch(...) {
        qWarning() << "QueryThread::doRequest: unhandled exception";
    }
	req->finished();
}

void 
QueryThread::enqueue(ITrackResolveRequest* req)
{
    Q_ASSERT(req);
    if (req) {
        m_mutex.lock();
        m_queue.append(req);
        m_wakeUp.wakeAll();
        m_mutex.unlock();
    }
}


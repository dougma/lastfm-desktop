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
#include <QThreadPool>

extern QString remapVolumeName(const QString& volume);


TrackResolver::TrackResolver()
: m_queryPool(0)
, m_scanner(0)
, m_bStopping(false)
, m_collection(LocalCollection::create("TrackResolverConnection"))
{}

TrackResolver::~TrackResolver()
{
    delete m_collection;
    delete m_scanner;
    m_bStopping = true;     // cause runnables to exit
    delete m_queryPool;
}

void
TrackResolver::init()
{
    // 1 thread. 
    // Don't change; 
    // m_collection not thread-safe
    m_queryPool = new QThreadPool();
    m_queryPool->setMaxThreadCount(1);      

    m_scanner = new LocalContentScanner;
}

void 
TrackResolver::resolve(class ITrackResolveRequest *req)
{
    Q_ASSERT(m_queryPool && req);
    if (req) {
        if (m_queryPool)
	        m_queryPool->start(new RequestRunnable(this, m_collection, req, m_dbPath));
        else
            req->finished();
    }
}

void 
TrackResolver::finished()
{
    delete this;
}

bool
TrackResolver::stopping()
{
    return m_bStopping;
}

//////////////////////////////////////////////////////////////////////

    
TrackResolver::Response::Response(const LocalCollection::ResolveResult &r)
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
TrackResolver::Response::matchQuality() const
{
    return m_matchQuality;
}

const char *
TrackResolver::Response::url() const
{
    return m_url.constData();
}

const char *
TrackResolver::Response::artist() const
{
    return m_artist.constData();
}

const char *
TrackResolver::Response::album() const
{
    return m_album.constData();
}

const char *
TrackResolver::Response::title() const
{
    return m_title.constData();
}

const char *
TrackResolver::Response::filetype() const
{
    return "audio/mpeg";    // yes this is mp3, todo: the others
}

unsigned 
TrackResolver::Response::duration() const
{
    return m_duration;
}

unsigned 
TrackResolver::Response::kbps() const
{
    return m_kbps;
}


void 
TrackResolver::Response::finished()
{
    delete this;
}


//////////////////////////////////////////////////////////////////////


TrackResolver::RequestRunnable::RequestRunnable(TrackResolver *trackResolver, LocalCollection* collection, ITrackResolveRequest *r, const QString &dbPath)
:   m_trackResolver(trackResolver),
    m_collection(collection),
    m_req(r),
    m_dbPath(dbPath)
{}

void
TrackResolver::RequestRunnable::run()
{
	try 
	{
        if (!m_trackResolver->stopping()) {
            QTime start(QTime::currentTime());
            QList<LocalCollection::ResolveResult> results = 
                m_collection->resolve(
                    QString::fromUtf8(m_req->artist()),
                    QString::fromUtf8(m_req->album()),
                    QString::fromUtf8(m_req->title()));

            foreach (const LocalCollection::ResolveResult &r, results) {
                QString filename(r.m_sourcename + r.m_path + r.m_filename);
                if (true /*bLooksReadable*/)
                    m_req->result(new Response(r));
            }
            qDebug() << "resolve complete: " << start.msecsTo(QTime::currentTime()) << "ms";
        }
	} 
	catch(QSqlError &e)
	{
        // todo
	}
    catch(...)
    {

    }
	m_req->finished();
}

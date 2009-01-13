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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "TrackResolverThread.h"
#include "TrackResolver.h"
#include "QueryError.h"
#include <QDebug>


TrackResolverThread*
TrackResolverThread::create()
{
    TrackResolverThread* result = new TrackResolverThread();
    result->start();
    return result;
}

void 
TrackResolverThread::run()
{
    try {
        m_pCollection = LocalCollection::create("TrackResolver");
        TRequestThread<ITrackResolveRequest>::run();
    } 
    catch (QueryError &e) {
        qCritical() << "TrackResolver::run: " + e.text();
    }
    catch (...) {
        qCritical() << "TrackResolver::run: unhandled exception";
    }

    try {
        // finish any remaining requests without actually doing anything
        ITrackResolveRequest* req;
        while (0 != (req = takeNextFromQueue())) {
            req->finished();
        }

        delete m_pCollection;
        m_pCollection = 0;
    }
    catch (...) {
        qCritical() << "QueryThread::run: unhandled exception";
    }
}


void
TrackResolverThread::doRequest(ITrackResolveRequest* req)
{
	try 
	{
        QTime start(QTime::currentTime());
        QList<LocalCollection::ResolveResult> results = 
            m_pCollection->resolve(
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



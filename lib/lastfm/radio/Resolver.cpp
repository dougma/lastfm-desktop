/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *    This program is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "Resolver.h"
#include "lastfm/types/Track.h"


Resolver::Resolver( const QList<ITrackResolverPlugin*>& plugins)
        : m_plugins(plugins)
{
    // ResolveAttempt needs these types registered for its queued signals
    qRegisterMetaType<Track>("Track");
    qRegisterMetaType<ITrackResolveResponse *>("ITrackResolveResponse *");

    foreach(ITrackResolverPlugin* p, plugins) {
        p->init();
    }
}


ResolveAttempt *
Resolver::create(const Track& track) const
{
    ResolveAttempt* reply = new ResolveAttempt();
    int refcount = m_plugins.size();
    if (refcount) {
        reply->m_req = new ResolveRequest(track, reply, refcount);
    }
    return reply;
}

void Resolver::submit(const ResolveAttempt* r) const
{
    if (r->m_req) {
        foreach(ITrackResolverPlugin *p, m_plugins) {
            p->resolve(r->m_req);
        }
    }
}


//////////////////////////////////////////////////////////////////////

ResolveAttempt::ResolveAttempt()
: m_req(0)
{
}

ResolveAttempt::~ResolveAttempt()
{
    delete m_req;
}

//////////////////////////////////////////////////////////////////////

ResolveRequest::ResolveRequest(const Track& t, ResolveAttempt* reply, unsigned refCount)
: m_track(t)
, m_reply(reply)
, m_ref(refCount)
, m_artist( QString(t.artist()).toUtf8() )
, m_album( QString(t.album()).toUtf8() )
, m_title( t.title().toUtf8() )
, m_mbid( QString(t.mbid()).toUtf8() )
, m_fpid( QString(t.fingerprintId()).toUtf8() )
{
}

ResolveRequest::~ResolveRequest()
{
    foreach(ITrackResolveResponse *r, m_responses) {
        r->finished();
    }
}

const char *
ResolveRequest::artist() const
{
    return m_artist.constData();    
}

const char *
ResolveRequest::album() const
{
    return m_album.constData();
}

const char *
ResolveRequest::title() const
{
    return m_title.constData();
}

const char *
ResolveRequest::lastfm_fpid() const
{
    return m_fpid.constData();
}

const char *
ResolveRequest::musicbrainz_trackid() const
{
    return m_mbid.constData();
}

const char *
ResolveRequest::puid() const
{
    return "";
}

void 
ResolveRequest::result(class ITrackResolveResponse *resp)
{
    m_responses << resp;    // we take ownership
    emit m_reply->resolveResponse(m_track, resp);
}

void 
ResolveRequest::finished()
{
    if (0 == --m_ref) {
        emit m_reply->resolveComplete(m_track);
    }
}


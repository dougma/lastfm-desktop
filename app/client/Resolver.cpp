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
    // needs these for queued signals
    qRegisterMetaType<Track>("Track");
    qRegisterMetaType<ITrackResolveResponse *>("ITrackResolveResponse *");

    foreach(ITrackResolverPlugin* p, plugins) {
        p->init();
    }
}

Resolver::~Resolver()
{
    foreach(ITrackResolverPlugin* p, m_plugins) {
        p->finished();
    }
}


void
Resolver::resolve(const Track& track)
{
    if (!track.isNull()) {      // protect our sanity
        int refcount = m_plugins.size();
        if (refcount) {
            ResolveRequest *rr = new ResolveRequest(track, refcount);

            // we queue the signals back here to maintain our
            // promise of calling the plugins from the same thread
            connect(rr, SIGNAL(resolveResponse(const Track, ITrackResolveResponse *)), 
                SLOT(onResolveResponse(const Track, ITrackResolveResponse *)), Qt::QueuedConnection );
            connect(rr, SIGNAL(resolveComplete(const Track)), 
                SLOT(onResolveComplete(const Track)), Qt::QueuedConnection );

            // ResolveRequest is deleted when the last plugin callsback on ResolveRequest::finished()
            
            foreach(ITrackResolverPlugin *p, m_plugins) {
                p->resolve(rr);
            }
            m_active.insert(MappableTrack(track), -1.0);
        }
    }
}

void
Resolver::onResolveResponse(const Track t, ITrackResolveResponse *resp)
{
    if (m_active.contains(t) && resp->matchQuality() > m_active.value(t)) {
        // This response is the best one so far, so
        // mutate the track right now...
        // or should we push this out to the class user?
        // well they did trust us to resolve it so we'll do it here
        m_active[t] = resp->matchQuality();
        QString localContent(QString::fromUtf8(resp->url()));

        qDebug() << "local content!!!: " + localContent;

        MutableTrack mt(t);
        mt.setDuration(resp->duration());
        mt.setUrl(QUrl(localContent));
    }
    resp->finished();
}

void 
Resolver::onResolveComplete(const Track t)
{
    if (m_active.contains(MappableTrack(t))) {
        m_active.remove(MappableTrack(t));
        emit resolveComplete(t);
    }
}

bool
Resolver::stopResolving(const Track& t) 
{
    return 0 == m_active.remove(MappableTrack(t));
}

bool
Resolver::stillResolving(const Track& t)
{
    return m_active.contains(MappableTrack(t));
}

//////////////////////////////////////////////////////////////////////


ResolveRequest::ResolveRequest(const Track& t, unsigned refCount)
: m_track(t)
, m_artist(QString(t.artist()).toUtf8())
, m_album(QString(t.album()).toUtf8())
, m_title(t.title().toUtf8())
, m_mbid(QString(t.mbid()).toUtf8())
, m_fpid(QString(t.fingerprintId()).toUtf8())
, m_ref(refCount)
{
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
    emit resolveResponse(m_track, resp);
}

void 
ResolveRequest::finished()
{
    if (0 == --m_ref) {
        emit resolveComplete(m_track);
        delete this;
    }
}


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

#ifndef RESOLVER_H
#define RESOLVER_H

#include "lib/lastfm/radio/Radio.h"
#include "app/clientplugins/ITrackResolver.h"
#include <QList>
#include <QMap>

// makes Track work as a key in an associative container
class MappableTrack : public Track
{
public:
    MappableTrack(const Track& t)
        :Track(t)
    {
    }

    bool operator<(const MappableTrack& that) const
    {
        return this->d.data() < that.d.data();
    }
};


// Resolver class is used for starting the track resolution requests
class Resolver : public QObject
{
    Q_OBJECT;

    QList<ITrackResolverPlugin*> m_plugins;
    QMap<MappableTrack, float> m_active;    // maps tracks, to best quality match so far

public:
    Resolver(const QList<ITrackResolverPlugin*>& plugins);
    ~Resolver();

    void resolve(const Track &);

    /* prevents further signals for track t,
    returns true if resolving was complete for track t */
    bool stopResolving(const Track &);

    /* returns true if still resolving track t */
    bool stillResolving(const Track &);

signals:
    void resolveComplete(const Track);

private slots:
    void onResolveResponse(const Track, ITrackResolveResponse *);
    void onResolveComplete(const Track);
};


// This is the request object sent to the plugins
// Its lifetime is managed by the plugins (calling finished())
class ResolveRequest : public QObject, public ITrackResolveRequest
{
    Q_OBJECT;

    Track m_track;
    QByteArray m_artist;
    QByteArray m_album;
    QByteArray m_title;
    QByteArray m_mbid;
    QByteArray m_fpid;

    unsigned m_ref;         // may be referenced by multiple plugins

public:
    ResolveRequest(const Track&, unsigned refCount);

    virtual const char *artist() const;
    virtual const char *album() const;
    virtual const char *title() const;
    virtual const char *lastfm_fpid() const;
    virtual const char *musicbrainz_trackid() const;
    virtual const char *puid() const; 
    virtual void result(ITrackResolveResponse *resp);
    virtual void finished();

signals:
    void resolveResponse(const Track, ITrackResolveResponse *);
    void resolveComplete(const Track);
};


#endif

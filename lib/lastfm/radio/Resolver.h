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


// Resolver class is used for starting the track resolution requests
class LASTFM_RADIO_DLLEXPORT Resolver
{
    QList<ITrackResolverPlugin*> m_plugins;

public:
    Resolver( const QList<ITrackResolverPlugin*>& plugins);

    // Creates a ResolveReply (and submits the resolve request to
    // the plugins).  Returns NULL if there are no plugins.
    class ResolveReply* resolve(const Track &);
};


// ResolveReply represents an in-progress track resolution request; it
// generates signals on responses (there can be multiple) and on completion
class LASTFM_RADIO_DLLEXPORT ResolveReply : public QObject
{
    Q_OBJECT;
    Q_DISABLE_COPY(ResolveReply);

    class ResolveRequest* m_req;        // we own this

    ResolveReply();
    void setRequest(ResolveRequest*);

    friend class Resolver;              // Resolver constructs us
    friend class ResolveRequest;        // ResolveRequest emits our signals 

public:
    ~ResolveReply();

signals:
    void response(class ITrackResolveResponse *);
    void requestComplete();
};


// This is the request object sent to the plugins
// Its lifetime is managed by the plugins (calling finished())
class LASTFM_RADIO_DLLEXPORT ResolveRequest : public ITrackResolveRequest
{
    // Track m_track;   // would this be useful?
    QByteArray m_artist;
    QByteArray m_album;
    QByteArray m_title;
    QByteArray m_mbid;
    QByteArray m_fpid;

    unsigned m_ref;         // may be referenced by multiple plugins
    ResolveReply* m_reply;
    QList <ITrackResolveResponse*> m_responses; // we own these

public:
    ResolveRequest(const Track&, ResolveReply* reply, unsigned refCount);
    ~ResolveRequest();

    virtual const char *artist() const;
    virtual const char *album() const;
    virtual const char *title() const;
    virtual const char *lastfm_fpid() const;
    virtual const char *musicbrainz_trackid() const;
    virtual const char *puid() const; 
    virtual void result(class ITrackResolveResponse *resp);
    virtual void finished();
};


#endif

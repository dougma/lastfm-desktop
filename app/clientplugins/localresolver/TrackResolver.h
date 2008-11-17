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

#ifndef TRACK_RESOLVER_H
#define TRACK_RESOLVER_H

#include "../ITrackResolver.h"
#include "LocalCollection.h"
#include <QRunnable>


class TrackResolver : public ITrackResolverPlugin
{
    QString m_dbPath;
	class QThreadPool* m_queryPool;
	class LocalContentScanner* m_scanner;
    class LocalCollection* m_collection;
    bool m_bStopping;

    // each resolve request gets sent to the threadpool:
	class RequestRunnable : public QRunnable
	{
        QString m_dbPath;
		class ITrackResolveRequest* m_req;
        class LocalCollection* m_collection;
        class TrackResolver* m_trackResolver;
	public:
		RequestRunnable(class TrackResolver*, LocalCollection*, ITrackResolveRequest*, const QString& dbPath);
		void run();
	};

    // one of these turns a LocalCollection::ResolveResult into 
    // something 'simpler' for the plugin interface
    class Response : public ITrackResolveResponse
    {
        float m_matchQuality;
        QByteArray m_url;
        QByteArray m_artist;
        QByteArray m_album;
        QByteArray m_title;
        //QByteArray m_filetype;
        unsigned m_duration;
        unsigned m_kbps;
    public:
        Response(const LocalCollection::ResolveResult& r);
	    virtual float matchQuality() const;
	    virtual const char* url() const;
	    virtual const char* artist() const;
	    virtual const char* album() const;
	    virtual const char* title() const;
	    virtual const char* filetype() const;
	    virtual unsigned duration() const;
	    virtual unsigned kbps() const;
	    virtual void finished();
    };

public:
    TrackResolver();
	~TrackResolver();

    virtual void init();
	virtual void resolve(class ITrackResolveRequest* req);
    virtual void finished();
    bool stopping();
};


#endif

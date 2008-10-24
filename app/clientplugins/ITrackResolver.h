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

#ifndef ITRACK_RESOLVER_H
#define ITRACK_RESOLVER_H

class ITrackResolverPlugin
{
public:
    virtual void init() = 0;
	virtual void resolve(class ITrackResolveRequest *req) = 0;
    virtual void finished() = 0;
};


class ITrackResolveRequest
{
public:
	virtual const char *artist() const = 0;
	virtual const char *album() const = 0;
	virtual const char *title() const = 0;
    virtual const char *lastfm_fpid() const = 0;
	virtual const char *musicbrainz_trackid() const = 0;
	virtual const char *puid() const = 0; 
	virtual void result(class ITrackResolveResponse *resp) = 0;
	virtual void finished() = 0;
};


class ITrackResolveResponse
{
public:
	virtual float matchQuality() const = 0;
	virtual const char *url() const = 0;
	virtual const char *artist() const = 0;
	virtual const char *album() const = 0;
	virtual const char *title() const = 0;
	virtual const char *filetype() const = 0;
	virtual unsigned duration() const = 0;
	virtual unsigned kbps() const = 0;
	virtual void finished() = 0;
};

#endif

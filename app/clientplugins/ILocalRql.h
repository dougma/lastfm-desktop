/***************************************************************************
 *   Copyright 2007-2009 Last.fm Ltd.                                      *
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


#ifndef I_LOCAL_RQL_H
#define I_LOCAL_RQL_H

#pragma GCC system_header


class ILocalRqlPlugin
{
public:
    virtual void init() = 0;
	virtual void parse(const char *rql, class ILocalRqlParseCallback *) = 0;
    virtual void testTag(const char *url) = 0;
    virtual void finished() = 0;
};


class ILocalRqlParseCallback
{
public:
    virtual void parseOk(class ILocalRqlTrackSource*, unsigned trackCount) = 0;
    virtual void parseFail(int errorLineNumber, const char *errorLine, int errorOffset) = 0;
};


class ILocalRqlTrackSource
{
public:
    virtual void getNextTrack(class ILocalRqlTrackCallback*) = 0;
    virtual void finished() = 0;
};


class ILocalRqlTrackCallback
{
public:
    virtual void trackFail() = 0;
    virtual void trackOk(
        const char* title,
        const char* album,
        const char* artist,
        const char* url,
        unsigned duration) = 0;
};



#endif
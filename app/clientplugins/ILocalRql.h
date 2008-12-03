/***************************************************************************
 *   Copyright 2007-2008 Last.fm Ltd.                                      *
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


#ifndef ILOCALRQL_H
#define ILOCALRQL_H

// callback interfaces are only valid for the duration of the call


class ILocalRqlTrackCallback
{
public:
    virtual void title(const char*) = 0;
    virtual void album(const char*) = 0;
    virtual void artist(const char*) = 0;
    virtual void url(const char*) = 0;
    virtual void duration(unsigned) = 0;
};

class ILocalRqlParseCallback
{
public:
    virtual void parseOk(class ILocalRqlTrackSource*) = 0;
    virtual void parseFail(int errorLineNumber, const char *errorLine, int errorOffset) = 0;
};


// these ones need to be finished() when they're done with

class ILocalRqlTrackSource
{
public:
    virtual unsigned tracksLeft() = 0;
    virtual bool nextTrack(ILocalRqlTrackCallback*) = 0;
    virtual void finished() = 0;
};

class ILocalRqlPlugin
{
public:
    virtual void init() = 0;
	virtual void parse(const char *rql, ILocalRqlParseCallback *) = 0;
    virtual void testTag(const char *url) = 0;
    virtual void finished() = 0;
};

#endif
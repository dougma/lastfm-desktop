/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef LOCAL_RQL_H
#define LOCAL_RQL_H

#include <lastfm/global.h>
#include <QObject>
#include "app/clientplugins/ILocalRql.h"


// This is the client's interface to the LocalRql plugin(s)
//
class LocalRql
{
    class ILocalRqlPlugin* m_plugin;

public:
    LocalRql(const QList<ILocalRqlPlugin*>& plugins);
    ~LocalRql();

    bool isAvailable();
    class LocalRqlResult* startParse(QString rql);
};


// RQL parsing results and track notifications are signalled via 
// this class as created by LocalRql::startParse
//
class LocalRqlResult 
    : public QObject
    , public ILocalRqlParseCallback
{
    Q_OBJECT

    class ILocalRqlTrackSource* m_trackSource;
    class LocalRqlNextTrack* m_nextTrack;

    // ILocalRqlParseCallback:
    void parseOk(class ILocalRqlTrackSource*, unsigned trackCount);
    void parseFail(int errorLineNumber, const char *errorLine, int errorOffset);

    LocalRqlResult();
    friend class LocalRql;          // LocalRql creates these objects

public:
    ~LocalRqlResult();

    class LocalRadioTrackSource* createLocalRadioTrackSource();
    void getNextTrack();

signals:
    void parseGood(unsigned);
    void parseBad(int, const QString&, int);
    void track(const Track& );
    void endOfTracks();
};


// This class implements ILocalRqlTrackCallback
// An object per callback allows us to properly manage its lifetime.
// This class should be internal to LocalRqlResult, but Qt's moc can't do that.
//
class LocalRqlTrackCallback
    : public QObject
    , public ILocalRqlTrackCallback
{
    Q_OBJECT

    // ILocalRqlTrackCallback:
    void trackOk(const char* title, const char* album, const char* artist, const char* url, unsigned duration);
    void trackFail();

signals:
    void track(const Track& );
    void endOfTracks();
};



#endif
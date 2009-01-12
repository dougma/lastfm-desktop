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

#ifndef LOCAL_RQL_H
#define LOCAL_RQL_H

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
    void testTag(QString url);
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
    void track(const class Track& );
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
    void track(const class Track& );
    void endOfTracks();
};



#endif
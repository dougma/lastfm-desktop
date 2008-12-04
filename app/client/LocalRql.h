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
#include "lib/lastfm/types/Track.h"
#include "app/clientplugins/ILocalRql.h"

class LocalRqlResult 
    : public QObject
    , public ILocalRqlParseCallback
    , public ILocalRqlTrackCallback
{
    Q_OBJECT

    class ILocalRqlTrackSource* m_trackSource;

    // these callbacks can occur on random (plugin) threads, 
    // so all they do is emit the signals below.
    // consequently these signals need to be wired up as queued
    // connections.  how to enforce this?

    // ILocalRqlParseCallback:
    void parseOk(class ILocalRqlTrackSource*, unsigned trackCount);
    void parseFail(int errorLineNumber, const char *errorLine, int errorOffset);

    // ILocalRqlTrackCallback:
    void trackOk(const char* title, const char* album, const char* artist, const char* url, unsigned duration);
    void trackFail();

public:
    LocalRqlResult();
    ~LocalRqlResult();

    class LocalRadioTrackSource* createLocalRadioTrackSource();
    void getNextTrack();

signals:
    void parseGood(unsigned);
    void parseBad(int, QString, int);
    void track(Track);
    void endOfTracks();
};



class LocalRql
{
    class ILocalRqlPlugin* m_plugin;

public:
    LocalRql(const QList<ILocalRqlPlugin*>& plugins);
    ~LocalRql();

    bool isAvailable();
    LocalRqlResult* startParse(QString rql);
    void testTag(QString url);
};

#endif
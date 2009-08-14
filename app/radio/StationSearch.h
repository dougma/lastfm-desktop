/*
   Copyright 2005-2009 Last.fm Ltd. 

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

#ifndef STATION_SEARCH_H
#define STATION_SEARCH_H

#include <QObject>
#include <lastfm/RadioStation>

// Use radio.search to find the most likely station (tag or similar artist).
// Fallback to searching friends list for library radio if radio.search 
// returns no result.
class StationSearch : public QObject
{
    Q_OBJECT;

public:
    void startSearch(const QString& name);

signals:
    void searchResult(RadioStation);

private slots:
    void onFinished();
    void onUserGotFriends();

private:
    QString m_name;
};

#endif

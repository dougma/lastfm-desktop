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
#ifndef PLAY_LIST_H
#define PLAY_LIST_H

#include <QList>
#include <QString>
#include <QObject>
#include "playdar/BoffinRqlRequest.h"

class PlaydarConnection;

typedef QList<BoffinPlayableItem> ItemList;

class Playlist : public QObject
{
    Q_OBJECT

public:
    Playlist(QObject *parent = 0);
    void startRequest(const QString& rql, PlaydarConnection* pc);

private slots:
    void onItem(const BoffinPlayableItem& item);
    void onAfterDelay();

private:
    int m_lastSize;

    ItemList m_playlist;       // shuffled into here
};

#endif

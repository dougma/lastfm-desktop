/*
   Copyright 2009 Last.fm Ltd. 
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
#ifndef TRACK_SOURCE_H
#define TRACK_SOURCE_H

#include <QList>
#include <lastfm/Track>
#include "playdar/BoffinPlayableItem.h"

class Shuffler;

// Tracksource samples BoffinPlayableItem objects from the Shuffler, 
// maintains a small buffer of them (for upcoming-track feature).
// MediaPipeline then takes Track objects from us.
class TrackSource
    : public QObject
{
    Q_OBJECT

public:
    TrackSource(Shuffler* shuffler, QObject *parent);

    Track takeNextTrack();
    BoffinPlayableItem peek(unsigned index);
    int size();
    void setSize(unsigned maxSize);
    void clear();

signals:
    void changed();     // the buffer has changed somehow.

private:
    void fillBuffer();

    Shuffler* m_shuffler;
    QList<BoffinPlayableItem> m_buffer;
    int m_maxSize;
};

#endif

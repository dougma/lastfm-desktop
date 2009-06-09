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
#ifndef LOCAL_RADIO_TRACK_SOURCE_H
#define LOCAL_RADIO_TRACK_SOURCE_H

#include <lastfm/AbstractTrackSource>


class LocalRadioTrackSource : public AbstractTrackSource
{
    Q_OBJECT;

    class LocalRqlResult* m_rqlResult;
    QList<Track> m_buffer;
    bool m_waiting;         // we are waiting for a callback from m_rqlResult
    bool m_endReached;      // m_rqlResult has signalled endOfTracks

private slots:
    void onTrack(Track);
    void onEndOfTracks();

public:
    // takes ownership of rqlResult:
    LocalRadioTrackSource(class LocalRqlResult* rqlResult);
    ~LocalRadioTrackSource();

    virtual Track takeNextTrack();
    void start();
};

#endif

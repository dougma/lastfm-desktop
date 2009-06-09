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
#ifndef RESOLVING_TRACK_SOURCE_H
#define RESOLVING_TRACK_SOURCE_H

#include <lastfm/AbstractTrackSource>
#include <QList>


// Provides 'track resolving' functionality by
// wrapping up another track source.
// 
class ResolvingTrackSource : public AbstractTrackSource
{
    Q_OBJECT

    class Resolver* m_resolver;
    AbstractTrackSource* m_src;
    QList<Track> m_queue;
    Track m_delaying;
    bool m_firstDelay;
    bool m_waiting;
    bool m_started;

    unsigned m_minQueue;
    unsigned m_maxQueue;
    int m_initialResolveDelay;
    int m_interTrackResolveDelay;
    
    void fill();
    void startResolveTimer(Track t);

private slots:
    void onTrackAvailable();
    void onResolveComplete(const Track);
    void onResolveTimeout();

public:
    ResolvingTrackSource(class Resolver* resolver, AbstractTrackSource *src);
    ~ResolvingTrackSource();
    void setMaxQueueSize(unsigned);
    void setMinQueueSize(unsigned);
    void setInitialResolveDelay(int);
    void setInterTrackResolveDelay(int);
    void start();

    virtual Track takeNextTrack();
};

#endif
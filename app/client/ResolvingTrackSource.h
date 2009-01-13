/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#ifndef RESOLVING_TRACK_SOURCE_H
#define RESOLVING_TRACK_SOURCE_H

#include "lib/lastfm/radio/AbstractTrackSource.h"
#include <QList>

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
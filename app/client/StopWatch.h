/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd                                       *
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

#ifndef STOP_WATCH_H
#define STOP_WATCH_H

#include "lib/scrobble/ScrobblePoint.h"
#include <QThread>
#include <QMutex>
#include <QDateTime>

//TODO arbiturary start times
//TODO hideously inefficient, though of course not bad, but at the least we 
// should stop waking every 250ms when the player is paused


/** exists so we can destroy stop watches without blocking the GUI thread in
  * wait() */
class StopWatchThread : public QThread
{
    Q_OBJECT
    Q_DISABLE_COPY( StopWatchThread )
    ~StopWatchThread() {} // use deleteLater()

    bool m_done;

public:
    StopWatchThread() : m_done( false ), m_paused( true ), m_timeout( 0 ), m_elapsed( 0 )
    {}

    virtual void run();
    void deleteLater();

    bool m_paused;
    /** all in units of milliseconds */
    uint m_timeout;
    uint m_elapsed;

signals:
    void tick( int ); // int is the usual signal/slot parameter, uint breaks
    void timeout();
};


/** Emits timeout() after seconds specified to start. 
  * Emits tick every second.
  * Continues to measure time after that point until object death.
  */
class StopWatch : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( StopWatch )

public:
    /** the StopWatch starts off paused, call resume() to start */
    StopWatch( const ScrobblePoint& timeout_in_seconds );
    ~StopWatch();
    
    void pause();
    void resume();

    uint elapsed() { return m_thread->m_elapsed / 1000; }
    
    ScrobblePoint scrobblePoint() const { return m_point; }

signals:
    void resumed();
    void paused();
    void tick( int );
    void timeout();

private:
    StopWatchThread* m_thread;
    ScrobblePoint m_point;
};

#endif

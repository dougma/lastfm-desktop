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
#ifndef STOP_WATCH_H
#define STOP_WATCH_H

#include <lastfm/ScrobblePoint>
#include <QDateTime>
#include <QObject>
#include <QTimer>
namespace audioscrobbler { class Application; }

namespace mxcl
{
    struct Time : private QTime
    {
        Time() : started( false ) 
        {}
        
        bool started;
        
        void restart() { started = true; QTime::restart(); }

        /** Disallow negative time, Trolltech suck? */
        uint elapsed() const 
        { 
            return started ? qMax( 0, QTime::elapsed() ) : 0;
        }
    };
}


/** Emits timeout() after seconds specified to start. 
  * Continues to measure time after that point until object death.
  */
class StopWatch : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( StopWatch )

    friend class audioscrobbler::Application; //for access to timeout() signal 
    friend class TestStopWatch; //for testing, duh!
    
public:
    /** The StopWatch starts off paused, call resume() to start.
      * The watch will not timeout() if elapsed is greater that the 
      * scrobble point */
    StopWatch( ScrobblePoint timeout_in_seconds, uint elapsed_in_ms = 0 );
    ~StopWatch();

    bool isTimedOut() const { return m_remaining == 0 && !m_timer->isActive(); }

    void pause();
    void resume();
    
    /** in milliseconds */
    uint elapsed() const { return ((m_point*1000 - m_remaining) + ( m_timer->isActive() ? m_elapsed.elapsed() : 0)); }

    ScrobblePoint scrobblePoint() const { return m_point; }
    
signals:
    void paused( bool );
    void timeout();

private slots:
    void finished();
    
private:
    void start();
    
    class QTimer* m_timer;
    uint m_remaining;
    mxcl::Time m_elapsed;
    ScrobblePoint m_point;
};

#endif

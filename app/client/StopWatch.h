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

#include "lib/lastfm/scrobble/ScrobblePoint.h"
#include <QDateTime>
#include <QObject>
#include <QTimer>


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
    
    bool isFinished() const { return m_remaining == 0 && !m_timer->isActive(); }
    
    void pause();
    void resume();
    
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
    QTime m_elapsed;
    ScrobblePoint m_point;
};

#endif

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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include "lib/lastfm/scrobble/ScrobblePoint.h"
#include <QBasicTimer>
#include <QObject>
#include <QTime>


class PausableTimer : public QObject
{
    Q_OBJECT
    
    QBasicTimer timer;
    QTime elapsed;
    uint const interval;
    uint next_interval;
    uint iterations;
    
    static const uint STEPS = 23;
    
public:
    PausableTimer( const ScrobblePoint& p ) 
            : interval( (p*1000) / STEPS ),
              next_interval( interval ),
              iterations( 0 )
    {
        start( interval );
    }
    
public slots:
    void setPaused( bool b )
    {
        Q_ASSERT( iterations < STEPS );
        if (b != timer.isActive()) return;
        
        if (b) {
            timer.stop();
            next_interval = qMin( next_interval - elapsed.elapsed(), next_interval );
        }
        else
            start( next_interval );
    }
    
signals:
    void timeout();

private:
    void start(uint const time)
    {
        elapsed.start();
        timer.start(time, this);
    } 
    
    virtual void timerEvent(QTimerEvent*)
    {
        emit timeout();
        
        if (++iterations >= STEPS)
            timer.stop();
        else if (next_interval != interval)
            // we don't just start() every interval, as this introduces creep
            start(next_interval = interval);
        else
            elapsed.start();
    }
};

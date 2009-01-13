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

#include "lib/lastfm/scrobble/ScrobblePoint.h"
#include <QBasicTimer>
#include <QObject>
#include <QTime>
#include <QPixmap>
#include <QPointer>


static inline QPixmap scrobbleButtonPixmap( uint const i )
{
    Q_ASSERT( i > 0 );
    Q_ASSERT( i < 37 );
    QString const path = QString(":/ScrobbleButton/%1.png").arg( i, 2, 10, QChar('0') );
    return QPixmap( path );
}


class Glow : public QObject, QBasicTimer
{
    Q_OBJECT
    int value;
    int const maxvalue;
    
    virtual void timerEvent( QTimerEvent* )
    {
        emit pixmap( scrobbleButtonPixmap( 25 + (value % 12) ) );
        
        if (value++ == maxvalue)
            stop(),
            deleteLater();
    }
    
public:
    Glow( uint iterations ) : value( 0 ), maxvalue( iterations * 12 + 5 )
    {
        start( 37, this );
    }
    
signals:
    void pixmap( const QPixmap& );
};


class ScrobbleButtonAnimation : public QObject
{
    Q_OBJECT
    
    QBasicTimer timer;
    QTime elapsed;
    uint const interval;
    uint next_interval;
    uint frame;
    QPixmap m_pixmap;
    QPointer<Glow> m_glow;
    
    static const uint STEPS = 24;
    
public:
    ScrobbleButtonAnimation( uint from /* in milliseconds */, ScrobblePoint to ) 
            : interval( (to*1000) / STEPS )
            , next_interval( interval )
            , frame( 1 )
    {
        if (from >= to * 1000) 
            frame = STEPS;

        else if (from > 0) {
            frame = from / interval;
            start( interval - (from % interval) );
        }
        else
            start( interval );

        m_pixmap = scrobbleButtonPixmap( frame );
    }

    QPixmap pixmap() const { return m_pixmap; }
    bool done() const { return frame >= STEPS; }
    
public slots:
    void setPaused( bool b )
    {
        if (done()) return;
        if (b != timer.isActive()) return;
        
        if (b) {
            timer.stop();
            next_interval = qMin( next_interval - elapsed.elapsed(), next_interval );
        }
        else
            start( next_interval );
    }
    
    void glow( uint iterations = 3 )
    {
        if (!m_glow)
        {
            m_glow = new Glow( iterations ); // glow deletes self, leave it thus or you'll get animation b0rkage
            connect( m_glow, SIGNAL(pixmap( QPixmap )), SIGNAL(pixmap( QPixmap )) );
        }
    }
    
signals:
    void pixmap( const QPixmap& );

private:
    void start(uint const time)
    {
        elapsed.start();
        timer.start(time, this);
    } 
    
    virtual void timerEvent(QTimerEvent*)
    {
        m_pixmap = scrobbleButtonPixmap( frame++ );
        emit pixmap( m_pixmap );
        
        if (done())
            timer.stop();
        else if (next_interval != interval)
            // we don't just start() every interval, as this introduces creep
            start(next_interval = interval);
        else
            elapsed.start();
    }
};

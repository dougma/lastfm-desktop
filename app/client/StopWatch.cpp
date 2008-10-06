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

#include "StopWatch.h"
#include <QDebug>


StopWatch::StopWatch( const ScrobblePoint& timeout ) : m_thread( 0 ), m_point( timeout )
{
    m_thread = new StopWatchThread;
    connect( m_thread, SIGNAL(tick( int )), SIGNAL(tick( int )) );
    connect( m_thread, SIGNAL(timeout()), SIGNAL(timeout()) );
    
    m_thread->m_timeout = timeout * 1000;
    m_thread->start();
}


StopWatch::~StopWatch()
{
    if (m_thread) m_thread->deleteLater();
}


void
StopWatch::pause()
{
    m_thread->m_paused = true; 
    emit paused( true );
}

void
StopWatch::resume() 
{
    emit tick( m_thread->m_elapsed / 1000 );
    m_thread->m_paused = false; 
    emit paused( false );
}


void
StopWatchThread::run()
{
    QDateTime lastTime = QDateTime::currentDateTime();
    bool timedout = false;

    while( !m_done )
    {
        msleep( 250 );

        QDateTime const now = QDateTime::currentDateTime();

        if (m_paused) 
        {
            lastTime = now;
            continue;
        }

        int msSpentSleeping = lastTime.time().msecsTo( now.time() );

        // HACK: This is for when we pass midnight, QTime only works with days
        // so the msSpentSleeping will be a huge negative number. Force it
        // to 1000 and everything will be fine.
        if ( msSpentSleeping < 0 )
            msSpentSleeping = 1000;

        if ( msSpentSleeping >= 1000 )
        {
            lastTime = now;
            m_elapsed += msSpentSleeping;

            emit tick( m_elapsed / 1000 );

            if (!timedout && m_elapsed >= m_timeout)
            {
                emit timeout();
                timedout = true;
            }
        }
    }
}


void
StopWatchThread::deleteLater()
{
    if (isRunning())
    {
        m_done = true;
        connect( this, SIGNAL(finished()), SLOT(deleteLater()) );
    }
    else
        QObject::deleteLater();
}

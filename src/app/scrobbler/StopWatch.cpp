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


StopWatch::StopWatch()
{
    m_thread = new StopWatchThread;
}


StopWatch::~StopWatch()
{
    m_thread->deleteLater();
}


void
StopWatch::start( int const i )
{
    Q_ASSERT( !m_thread->isRunning() );
    
    m_thread->m_timeout = i;
    m_thread->start();
}


void
StopWatchThread::run()
{
    QDateTime lastTime = QDateTime::currentDateTime();
    int timeSoFar = 0;
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
            timeSoFar += msSpentSleeping;

            int s = timeSoFar / 1000;
            emit tick( s );

            if (s >= m_timeout)
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

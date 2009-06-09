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
#include "StopWatch.h"
#include <QTimer>


StopWatch::StopWatch( ScrobblePoint timeout, uint elapsed ) : m_point( timeout )
{    
    m_timer = new QTimer( this );
    m_timer->setSingleShot( true );
    m_remaining = qMax( int(m_point)*1000 - int(elapsed), 0 );

    connect( m_timer, SIGNAL(timeout()), SLOT(finished()) );
}

StopWatch::~StopWatch()
{
    if (!isTimedOut() && (m_point*1000) - elapsed() < 4000)
        emit timeout();
}


void
StopWatch::start() //private
{
    m_elapsed.restart();
    m_timer->setInterval( m_remaining );
    m_timer->start();
}


void
StopWatch::pause()
{
    if (!m_timer->isActive() || !m_remaining)
        return;

    m_timer->stop();
    
    // cater to potentially having more elapsed time than remaining time
    uint const remaining = m_remaining - m_elapsed.elapsed();
    m_remaining = (remaining <= m_remaining) ? remaining : 0;

    emit paused( true );
}


void
StopWatch::resume()
{
    if (!m_remaining || m_timer->isActive())
        return;

    start();
    emit paused( false );
}


void
StopWatch::finished()
{
    m_remaining = 0;
    emit timeout();
}

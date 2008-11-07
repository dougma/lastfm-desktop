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
#include <QTimer>


StopWatch::StopWatch( const ScrobblePoint& timeout ) : m_point( timeout )
{
    m_timer = new QTimer( this );
    m_timer->setSingleShot( true );
    m_remaining = m_point * 1000;

    connect( m_timer, SIGNAL(timeout()), SLOT(finished()) );
    start();
}


void
StopWatch::start()
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
    
    m_remaining -= m_elapsed.elapsed();
    m_timer->stop();
    emit paused( true );
}


void
StopWatch::resume()
{
    if (!m_remaining) 
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

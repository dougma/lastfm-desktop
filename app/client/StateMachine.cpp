/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *    This program is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include "StateMachine.h"


//TODO when a thing is paused and then its watch destroyed because a new active connection is spawned
//     save the elapsed time in the connection!


StateMachine::StateMachine( QObject* parent )
            : QObject( parent )
            , m_scrobbleFraction( 0.5 )
            , m_state( Stopped )
            , m_radioIsActive( false )
{}


void
StateMachine::setConnection( PlayerConnection* connection )
{    
    Q_ASSERT( connection );
    
    qDebug() << connection->id();
    
    if (connection == m_connection) return;
    
    if (m_connection)
    {
        m_connection->setElapsed( m_watch->elapsed() );
        disconnect( m_connection, 0, this, 0 );
    }
    
    m_connection = connection;
    
    if (m_radioIsActive) return;
    
    connect( connection, SIGNAL(trackStarted( Track )), SLOT(onTrackStarted( Track )) );
    connect( connection, SIGNAL(paused()), SLOT(pause()) );
    connect( connection, SIGNAL(resumed()), SLOT(resume()) );
    connect( connection, SIGNAL(stopped()), SLOT(stop()) );
    
    unspoolTrack();
    
    
    emit playerChanged( connection->name() );
    
    switch (connection->state())
    {
        case Buffering:
            spoolTrack( connection->track() );
            break;
            
        case Playing:
            spoolTrack( connection->track(), connection->elapsed() );
            start();
            break;
            
        case TuningIn:
        case Stopped:
            stop();
            break;
            
        case Paused:
            spoolTrack( connection->track(), connection->elapsed() );
            if (m_watch->remaining() != 0)
                pause();
            break;
    }
}


void
StateMachine::onTrackStarted( const Track& t )
{
    Q_ASSERT( m_connection == sender() );
    
    m_state = Buffering; //HACK to stop the spoolTrack function from emitting stateChanged( Buffering )
    spoolTrack( t );
    start();
}


void
StateMachine::spoolTrack( const Track& t, uint const elapsed /* in milliseconds */ )
{   
    if (m_track == t) 
    { 
        qWarning() << "Trying to start the same track as last time, assuming programmer error and doing nothing";
        return;
    }
    
    unspoolTrack();
    
    if (t.isNull()) { stop(); return; } //FIXME will rejig active player! :(
    
    m_track = t;
    delete m_watch;
    
    ScrobblePoint timeout( t.duration() * m_scrobbleFraction );
    m_watch = new StopWatch( timeout, elapsed );
    connect( m_watch, SIGNAL(timeout()), SLOT(onStopWatchTimedOut()) );
    
    emit trackSpooled( m_track, m_watch );
    
    if (m_state != Buffering)
    {
        m_state = Buffering;
        emit stateChanged( m_state, m_track );
    }
}


void
StateMachine::start()
{   
    Q_ASSERT( m_watch );
    Q_ASSERT( !m_track.isNull() );
    
    if (m_track.isNull()) 
        return;    
    
    if (m_state != Playing)
    {
        m_state = Playing;
        emit stateChanged( m_state, m_track );
    }
    
    m_watch->resume();
}


void
StateMachine::pause()
{
    Q_ASSERT( m_watch );
    
    m_watch->pause();    
    
    if (m_state != Paused)
    {
        m_state = Paused;
        emit stateChanged( m_state, m_track );
    }
}


void
StateMachine::resume()
{
    Q_ASSERT( m_watch );
    Q_ASSERT( m_state == Paused );
    
    m_watch->resume();
    
    if (m_state != Playing)
    {
        m_state = Playing;
        emit stateChanged( m_state, m_track );
    }
}


void
StateMachine::stop()
{
    unspoolTrack();
    
    Q_ASSERT( m_track.isNull() );
    Q_ASSERT( m_watch == 0 );
    
    m_state = Stopped;
    emit trackSpooled( track(), 0 );
    emit stopped();
    emit stateChanged( Stopped, Track() );
}


void
StateMachine::unspoolTrack()
{
    Track oldtrack = m_track;
    if (oldtrack.isNull())
        return;
    
    // always scrobble if the track almost played enough time.
    // we do this because, the durations reported by media players are not
    // reliable. And some media players cross fade early. And also we have a 
    // resolution of one second, so errors of one second are very likely.
    // This usually only matters when the scrobble point is set to something
    // close to 100%.
    
    //FIXME ideally we'd only do this if the track changes naturally
    // but scrobsub has no facility to notify us of that.
    
    if (m_connection && m_watch && !m_watch->isTimedOut())
    {
        QDebug d = qDebug() << "Watch didn't timeout, checking if we should scrobble anyway..";
        
        uint const elapsed = m_watch->elapsed() / 1000;

        // cater to iTunes crossfade
        QString const id = m_connection->id();
        if (elapsed >= m_track.duration() - 12 
            && m_track.duration() >= ScrobblePoint::kScrobbleMinLength
            && (id == "osx" || id == "itw"))
            emit m_watch->timeout();
        
        // allow 4 seconds of leeway, to allow for various inaccuracies
        else if (elapsed + 4 > m_watch->scrobblePoint())
            emit m_watch->timeout();
        
        else
            d << "no";
    }
    
    delete m_watch; //do always just in case, QPointer will set to 0 for us
    
    m_track = Track();
    emit trackUnspooled( oldtrack );
    
	// indeed state remains playing/loading, as playing refers to a session,
	// where we are actively trying to play something. We are in a kind of
	// loading state
}


void
StateMachine::onStopWatchTimedOut()
{
    Q_ASSERT( !m_track.isNull() );
    // emit even if null as that seems least the route with the least badness
    emit scrobblePointReached( m_track ); 
}


void
StateMachine::onRadioTuningIn( const RadioStation& )
{
    // docs state that Radio can emit tuningIn up to twice during any one 
    // tuningIn period
    if (m_state == TuningIn) return;
    
    m_radioIsActive = true;

    //HACK allows us to keep m_connection, but not respond to it
    if (m_connection) {
        disconnect( m_connection, 0, this, 0 );
        if (m_watch) {
            m_connection->setElapsed( m_watch->elapsed() );
            delete m_watch;
        }
    }
    //HACK ends
        
    unspoolTrack();
    if (m_state != Stopped)
        emit trackSpooled( Track() );
    
    m_state = TuningIn;
    emit stateChanged( TuningIn );
}


void
StateMachine::onRadioTrackSpooled( const Track& newtrack )
{
    Q_ASSERT( m_radioIsActive );
    spoolTrack( newtrack );
}


void
StateMachine::onRadioTrackStarted( const Track& newtrack )
{
    Q_ASSERT( m_radioIsActive );
    Q_ASSERT( newtrack == m_track );
    start();
}


void
StateMachine::onRadioBuffering( int pc )
{
    Q_ASSERT( m_radioIsActive );
    qDebug() << "Buffer status" << pc << "%";
}


void
StateMachine::onRadioStopped()
{
    m_radioIsActive = false;

    //HACK begins, replays previous connection, if any
    PlayerConnection* connection = m_connection;
    m_connection = 0;
    if (connection) {
        setConnection( connection );
    }
    //HACK ends
    
    if (m_connection == 0) 
        stop();
}

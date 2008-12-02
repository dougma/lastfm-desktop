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

#include "PlayerMediator.h"
#include "Settings.h"

#define NEW_STOP_WATCH_MACRO() { \
    ScrobblePoint sp( m_track.duration() * moose::Settings().scrobblePoint() / 100 ); \
    m_watch = new StopWatch( sp ); \
    connect( m_watch, SIGNAL(timeout()), SLOT(onStopWatchTimedOut()) ); }


PlayerMediator::PlayerMediator( PlayerListener* listener )
             : QObject( (QObject*)listener ),
               m_state( Stopped ),
               m_radioIsActive( false )
{
    connect( (QObject*)listener, SIGNAL(playerCommand( PlayerConnection )), SLOT(onPlayerConnectionCommandReceived( PlayerConnection )) );
}


void
PlayerMediator::onPlayerConnectionCommandReceived( const PlayerConnection& connection )
{
    // FIXME: need to handle Init command not happening
    if (m_connection.name != connection.name &&
        connection.command != PlayerCommandParser::Init)
    {
        emit playerChanged( connection.name );
    }

    m_connection = connection;
    
    if (m_radioIsActive)
        return;


    switch (connection.command)
    {
        case PlayerCommandParser::Init:
            emit playerChanged( connection.name );
            break;

        case PlayerCommandParser::Start:
            endTrack();
            m_track = connection.track;
            NEW_STOP_WATCH_MACRO();
            emit trackSpooled( m_track, m_watch );
            m_watch->resume(); //do after trackSpooled() as will emit paused( bool ) now
            changeState( Playing );
            break;
        
        case PlayerCommandParser::Stop:
            stop();
            break;

        case PlayerCommandParser::Pause:
            if (m_watch) m_watch->pause();
            changeState( Paused );
            break;

        case PlayerCommandParser::Resume:
            if (m_watch) m_watch->resume();
            changeState( Playing );
            break;

        case PlayerCommandParser::Term:
            stop();
            m_connection = PlayerConnection();
            emit playerChanged( "" );
            break;

        case PlayerCommandParser::Bootstrap:
            // handled elsewhere
            break;
    }
}



void
PlayerMediator::onRadioTuningIn( const RadioStation& )
{    
    m_radioIsActive = true;

    endTrack();
    if (m_state != TuningIn && m_state != Stopped)
        emit trackSpooled( Track() );
    changeState( TuningIn );
}


void
PlayerMediator::onRadioTrackSpooled( const Track& newtrack )
{
    m_radioIsActive = true;
    
    endTrack();
    m_track = newtrack;
    NEW_STOP_WATCH_MACRO();
    emit trackSpooled( m_track, m_watch );
    changeState( Buffering );
}


void
PlayerMediator::onRadioTrackStarted( const Track& newtrack )
{
    m_radioIsActive = true;
    
    if (m_track != newtrack)
    {
        endTrack();
        m_track = newtrack;
        NEW_STOP_WATCH_MACRO();
        emit trackSpooled( m_track, m_watch );
    }
    
    m_watch->resume();
    
    changeState( Playing );
}


void
PlayerMediator::onRadioBuffering( int pc )
{
    //TODO
    qDebug() << "Buffer status" << pc << "%";
}


void
PlayerMediator::onRadioStopped()
{
    m_radioIsActive = false;
    stop();
    replay( m_connection );
}            


void
PlayerMediator::replay( const PlayerConnection& connection )
{   
    if (!connection.id.size())
    {
        emit playerChanged( "" );
        return; //connections must have valid id
    }

    if (!connection.track.isNull())
    {
        PlayerConnection c = connection;
        c.command = PlayerCommandParser::Init;
        onPlayerConnectionCommandReceived( c );

        //FIXME flickers
        c.command = PlayerCommandParser::Start;
        onPlayerConnectionCommandReceived( c );
        
        if (connection.state == Paused)
        {
            c.command = PlayerCommandParser::Pause;
            onPlayerConnectionCommandReceived( c );
        }
    }
}


void
PlayerMediator::endTrack()
{
    // always scrobble if the track almost played enough time.
    // we do this because, the durations reported by media players are not
    // reliable. And some media players cross fade early. And also we have a 
    // resolution of one second, so errors of one second are very likely.
    // This usually only matters when the scrobble point is set to something
    // close to 100%.
    
    //FIXME ideally we'd only do this if the track changes naturally
    // but scrobsub has no facility to notify us of that.
    
    if (m_watch && !m_watch->isTimedOut() && !m_track.isNull())
    {
        qDebug() << "Watch didn't timeout, checking if we should scrobble anyway..";
        
        uint const elapsed = m_watch->elapsed();
        
        // cater to iTunes crossfade
        if (elapsed >= m_track.duration() - 12 
                && m_track.duration() >= ScrobblePoint::kScrobbleMinLength
                && (m_connection.id == "osx" || m_connection.id == "itw"))
            emit m_watch->timeout();

        // allow 4 seconds of leeway, to allow for various inaccuracies
        else if (elapsed + 4 > m_watch->scrobblePoint())
            emit m_watch->timeout();
    }

    delete m_watch; //do always just in case
        
    if (m_track.isNull())
        return;
   
    Track oldtrack = m_track;
    m_track = Track();
    emit trackUnspooled( oldtrack );
	
	// indeed state remains playing/loading, as playing refers to a session,
	// where we are actively trying to play something. We are in a kind of
	// loading state
}


void
PlayerMediator::onStopWatchTimedOut()
{
    emit scrobblePointReached( m_track );
}


void
PlayerMediator::changeState( State newstate )
{
    State oldstate = m_state;    
    
    if (m_track.isNull() && !m_radioIsActive)
        switch (newstate)
        {
            case Stopped:
            case TuningIn:
                break;
                
            case Paused:
            case Buffering:
            case Playing:
                qWarning() << "Invalid state transition, from" << oldstate << "to" << newstate << "when m_track is Null";
                stop(); //warning, recursive
                return;
        }
    
    if (newstate != oldstate)
    {
        qDebug().nospace() << newstate << " (was " << oldstate << ')';        
        m_state = newstate;
        emit stateChanged( m_state, m_track );
    }
}


void
PlayerMediator::stop()
{
    endTrack();
    emit trackSpooled( m_track /** will be a null track */, m_watch /** will be 0 */ );
    emit stopped();
    changeState( Stopped );
}

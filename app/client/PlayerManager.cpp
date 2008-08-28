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

#include "PlayerManager.h"
#include "PlayerEvent.h"
#include "Settings.h"


PlayerManager::PlayerManager( PlayerListener* listener )
             : QObject( (QObject*)listener ),
               m_state( PlayerState::Stopped )
{
    QObject* o = (QObject*)listener;
    connect( o, SIGNAL(trackStarted( Track )), SLOT(onTrackStarted( Track )) );
    connect( o, SIGNAL(playbackEnded( QString )), SLOT(onPlaybackEnded( QString )) );
    connect( o, SIGNAL(playbackPaused( QString )), SLOT(onPlaybackPaused( QString )) );
    connect( o, SIGNAL(playbackResumed( QString )), SLOT(onPlaybackResumed( QString )) );
    connect( o, SIGNAL(playerConnected( QString )), SLOT(onPlayerConnected( QString )) );
    connect( o, SIGNAL(playerDisconnected( QString)), SLOT(onPlayerDisconnected( QString )) );
}


#define ONE_PLAYER_HACK( id ) if (m_playerId.size() && m_playerId != id ) return;

void
PlayerManager::onTrackStarted( const Track& t )
{   
	ONE_PLAYER_HACK( t.playerId() )
	using namespace PlayerState;
	
	if (t.isNull() && (m_state == Stopped || m_state == Playing))
	{
		qWarning() << "Empty TrackInfo object presented for PlaybackStarted notification, this is wrong!";
		emit event( PlayerEvent::PlaybackSessionEnded );
		return;
	}

	PlayerState::Enum oldState = m_state;
	m_state = Playing;
	
	if (oldState == Stopped) {
		Q_ASSERT( m_track.isNull() );
		emit event( PlayerEvent::PlaybackSessionStarted, t.playerId() );
	}
	else if (!m_track.isNull())
		emit event( PlayerEvent::TrackEnded, QVariant::fromValue( m_track ) );
	
    delete m_track.m_watch; //stuff is connected to it, break those connections

	m_track = t;
	m_track.m_watch = new StopWatch( t.duration() * The::settings().scrobblePoint() / 100 ); 
	connect( m_track.m_watch, SIGNAL(timeout()), SLOT(onStopWatchTimedOut()) );

	emit event( PlayerEvent::TrackStarted, QVariant::fromValue( m_track ) );
}


void
PlayerManager::onPlaybackEnded( const QString& id )
{
	ONE_PLAYER_HACK( id )
	using namespace PlayerState;
	
	switch (m_state)
	{
		case Stopped:
			qWarning() << "Ignoring request by connected player to set Stopped state again";
			return;
			
		case Playing:
		case Paused:
		case Stalled:
			break;
	}
	
	m_state = Stopped;
	
	if (!m_track.isNull())
	{
		emit event( PlayerEvent::TrackEnded, QVariant::fromValue( m_track ) );
		delete m_track.m_watch;
		m_track = ObservedTrack();
	}
	
	emit event( PlayerEvent::PlaybackSessionEnded );
}


void
PlayerManager::onPlaybackPaused( const QString& id )
{
	ONE_PLAYER_HACK( id )
	using namespace PlayerState;
	
	switch (m_state)
	{
		case Paused:
			qWarning() << "Ignoring request by connected player to set Paused state again";
		case Stopped:
			return;
			
		case Playing:
		case Stalled:
			break;
	}

	if (m_track.isNull())
		return;
	
	Q_ASSERT( m_track.m_watch );
	
	m_track.m_watch->pause();
	m_state = Paused;

	emit event( PlayerEvent::PlaybackPaused, QVariant::fromValue( m_track ) );
}


void
PlayerManager::onPlaybackResumed( const QString& id )
{
	ONE_PLAYER_HACK( id )
	using namespace PlayerState;
	
	switch (m_state)
	{
		case Playing:
			// no point as nothing would change
			qWarning() << "Ignoring request by connected player to resume playing track";
			return;
			
		case Stopped:
			qWarning() << "Ignoring request by connected player to resume null track";
			return;

		case Stalled:
		case Paused:
			break;
	}
	
    Q_ASSERT( m_track.m_watch );
	Q_ASSERT( !m_track.isNull() );
	
	m_state = Playing;
	m_track.m_watch->resume();

	emit event( PlayerEvent::PlaybackUnpaused, QVariant::fromValue( m_track ) );
}


void
PlayerManager::onPlayerConnected( const QString &id )
{
	if (m_playerId.size()) return;
	
	m_playerId = id;
    emit event( PlayerEvent::PlayerConnected, QVariant::fromValue( id ) );
}


void
PlayerManager::onPlayerDisconnected( const QString &id )
{
	ONE_PLAYER_HACK( id )
	
    emit event( PlayerEvent::PlayerDisconnected, QVariant::fromValue( id ) );

    // Implicit PlayerEvent::PlaybackEnded to avoid crashing / buggy media
    // players leaving the scrobbler in a playing state
#if 0
    onPlaybackEnded( id );
#endif
}


void
PlayerManager::onStopWatchTimedOut()
{
    MutableTrack( track() ).upgradeRating( Track::Scrobbled );
    emit event( PlayerEvent::ScrobblePointReached, QVariant::fromValue( track() ) );
}

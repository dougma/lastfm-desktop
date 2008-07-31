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


void
PlayerManager::ban()
{}


void
PlayerManager::love()
{}


void
PlayerManager::onTrackStarted( const Track& t )
{   
    delete m_track.m_watch; //stuff is connected to it, break those connections

    m_track = t;
    m_track.m_watch = new StopWatch( t.duration() * The::settings().scrobblePoint() / 100 ); 
    connect( m_track.m_watch, SIGNAL(timeout()), SLOT(onStopWatchTimedOut()) );

    handleStateChange( PlayerState::Playing, m_track );
}


//TODO really we should be doing more checking, like is the top player actually in the playing state? could be paused.
void
PlayerManager::onPlaybackEnded( const QString& id )
{
    delete m_track.m_watch;
    m_track = ObservedTrack();
    handleStateChange( PlayerState::Stopped );
}


void
PlayerManager::onPlaybackPaused( const QString& id )
{
    if (m_track.m_watch)
        m_track.m_watch->pause();

    handleStateChange( PlayerState::Paused );
}


void
PlayerManager::onPlaybackResumed( const QString& id )
{
    if (m_track.m_watch)
        m_track.m_watch->resume();

    handleStateChange( PlayerState::Playing );
}


void
PlayerManager::onPlayerConnected( const QString &playerId )
{
    emit event( PlayerEvent::PlayerConnected, QVariant::fromValue( playerId ) );
}


void
PlayerManager::onPlayerDisconnected( const QString &playerId )
{
    emit event( PlayerEvent::PlayerDisconnected, QVariant::fromValue( playerId ) );

    // Implicit PlayerEvent::PlaybackEnded to avoid crashing / buggy media
    // players leaving the scrobbler in a playing state
#ifdef NDEBUG
//    #error did you build all the new plugins and that?
    onPlaybackEnded( playerId );
#endif
}


void
PlayerManager::handleStateChange( PlayerState::Enum newState, const ObservedTrack& t )
{
    using namespace PlayerState;

    PlayerState::Enum oldState = m_state;
    m_state = newState;
    QVariant v = QVariant::fromValue( t );

    if (newState == Playing && t.isEmpty())
    {
        qWarning() << "Empty TrackInfo object presented for Playback notification, this is wrong!";
        emit event( PlayerEvent::PlaybackEnded );
        return;
    }

    switch (oldState)
    {
    case Playing:
        switch (newState)
        {
        case Playing:
            emit event( PlayerEvent::TrackChanged, v );
            break;
        case Stopped:
            emit event( PlayerEvent::PlaybackEnded );
            break;
        case Paused:
            emit event( PlayerEvent::PlaybackPaused );
            break;
        }
        break;

    case Stopped:
        switch (newState)
        {
        case Playing:
            emit event( PlayerEvent::PlaybackStarted, v );
            break;
        case Stopped:
            // do nothing
            break;
        case Paused:
            //TODO this shouldn't happen, but needs handling
            // perhaps we should just stay looking stopped? after all what else
            // can be done?
            break;
        }
        break;

    case Paused:
        switch (newState)
        {
        case Playing:
            emit event( PlayerEvent::PlaybackUnpaused, v );
            break;
        case Stopped:
            emit event( PlayerEvent::PlaybackEnded );
            break;
        case Paused:
            // do nothing;
            break;
        }
        break;
    }
}


void
PlayerManager::onStopWatchTimedOut()
{
    MutableTrack( track() ).setRatingFlag( Track::Scrobbled );
    emit event( PlayerEvent::ScrobblePointReached, QVariant::fromValue( track() ) );
}

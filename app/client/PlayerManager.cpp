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
#include "PlaybackEvent.h"
#include "Settings.h"


PlayerManager::PlayerManager( PlayerListener* listener )
             : QObject( (QObject*)listener ),
               m_state( PlaybackState::Stopped )
{
    QObject* o = (QObject*)listener;
    connect( o, SIGNAL(trackStarted( Track )), SLOT(onTrackStarted( Track )) );
    connect( o, SIGNAL(playbackEnded( QString )), SLOT(onPlaybackEnded( QString )) );
    connect( o, SIGNAL(playbackPaused( QString )), SLOT(onPlaybackPaused( QString )) );
    connect( o, SIGNAL(playbackResumed( QString )), SLOT(onPlaybackResumed( QString )) );
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
    QString const id = t.playerId();
    Player& p = *m_players[id];
    p.track = t;
    p.state = PlaybackState::Playing;
    StopWatch* watch = new StopWatch( t.duration() * The::settings().scrobblePoint() / 100 ); 
    delete p.track.m_watch; //stuff is connected to it, break those connections
    p.track.m_watch = watch;

    qDebug() << p.track.toString() << p.track.isEmpty();

    if (m_players.top()->id == id)
    {
        connect( watch, SIGNAL(timeout()), SLOT(onStopWatchTimedOut()) );
        handleStateChange( p.state, p.track );
    }
}


//TODO really we should be doing more checking, like is the top player actually in the playing state? could be paused.
void
PlayerManager::onPlaybackEnded( const QString& id )
{
    bool const isActive = m_players.top()->id == id;

    QString id_ = m_players.top()->id;

    // do before anything so the state emission we are about to do is reflected
    // here as well as elsewhere
    m_players.remove( id );

    if (isActive)
    {
        if (m_players.count())
        {
            handleStateChange( PlaybackState::Playing, m_players.top()->track );
        }
        else
            handleStateChange( PlaybackState::Stopped );
    }
}


void
PlayerManager::onPlaybackPaused( const QString& id )
{
    m_players[id]->track.m_watch->pause();

    if (m_players.top()->id == id)
        handleStateChange( PlaybackState::Paused );
}


void
PlayerManager::onPlaybackResumed( const QString& id )
{
    m_players[id]->track.m_watch->resume();

    if (m_players.top()->id == id)
        handleStateChange( PlaybackState::Playing );
}


void
PlayerManager::handleStateChange( PlaybackState::Enum newState, const ObservedTrack& t )
{
    using namespace PlaybackState;

    PlaybackState::Enum oldState = m_state;
    m_state = newState;

    if (newState == Playing && t.isEmpty())
    {
        qWarning() << "Empty TrackInfo object presented for Playback notification, this is wrong!";
    }

    switch (oldState)
    {
    case Playing:
        switch (newState)
        {
        case Playing:
            emit event( PlaybackEvent::TrackChanged, QVariant::fromValue( t ) );
            break;
        case Stopped:
            emit event( PlaybackEvent::PlaybackEnded );
            break;
        case Paused:
            emit event( PlaybackEvent::PlaybackPaused );
            break;
        }
        break;

    case Stopped:
        switch (newState)
        {
        case Playing:
            emit event( PlaybackEvent::PlaybackStarted, QVariant::fromValue( t ) );
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
            emit event( PlaybackEvent::PlaybackUnpaused );
            break;
        case Stopped:
            emit event( PlaybackEvent::PlaybackEnded );
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
    emit event( PlaybackEvent::ScrobblePointReached );
}
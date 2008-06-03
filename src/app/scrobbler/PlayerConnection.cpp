/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Christian Muehlhaeuser, Last.fm Ltd <chris@last.fm>                *
 *      Erik Jaelevik, Last.fm Ltd <erik@last.fm>                          *
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

#include "exceptions.h"
#include "PlayerConnection.h"
#include "Settings.h"

#include "lib/unicorn/logger.h"
#include "lib/moose/MooseCommon.h"

#include <algorithm>
#include <sstream>

using namespace std;


/******************************************************************************
    CPlayerConnection
******************************************************************************/
CPlayerConnection::CPlayerConnection(
    const QString& sID) :
        QObject(),
        meState(eStopped),
        msID(sID),
        mCurrentStopWatch( &mInternalStopWatch ),
        mbScrobblePointReached(false),
        mbReadyForSubmission(false),
        mbScrobblable(false)
{
}

/******************************************************************************
    ~CPlayerConnection
******************************************************************************/
CPlayerConnection::~CPlayerConnection()
{
}

/******************************************************************************
    OnStart
******************************************************************************/
void
CPlayerConnection::OnStart(
    const CPlayerCommand& cmd )
{
    // OnStop doesn't return until the stopwatch thread is dead.
    OnStop( cmd );

    // We don't strictly need this until we connect the stopwatch
    // signal further down but it's easier managed in this scope.
    QMutexLocker locker( &m_mutex );

    mLastSubmission = TrackInfo();
    mLastSubmission = cmd.mTrack;
    mLastSubmission.timeStampMe();
    mLastSubmission.setPlayerId( cmd.mPluginId );

    // User now always needed in TrackInfo for the scrobbler Now Playing to work
    mLastSubmission.setUsername( The::currentUsername() );

    int nGoal;
    
    // Work out how long we need to listen before submission 
    if ( mLastSubmission.duration() - MooseUtils::scrobbleTime( mLastSubmission ) < 12 && 
         ( msID == "itw" || msID == "osx" ) )
        // iTunes supports gapless playback with fading time up to 12 seconds, therefore we 
        // have to subtract these seconds because otherwise the track gets never scrobbled.
        nGoal = MooseUtils::scrobbleTime( mLastSubmission ) - 13;
    else
        // Other players. - 5 because some duration in our databases may not be correct and +/- 1-5 seconds.
        nGoal = MooseUtils::scrobbleTime( mLastSubmission ) - 5;

    LOG(3, "Starting new track '" << mLastSubmission.toString() <<
           "' Goal: " << nGoal << "\n");

    if ( mLastSubmission.stopWatch() != 0 )
    {
        // Use radio timer
        mCurrentStopWatch = mLastSubmission.stopWatch();
    }
    else
    {
        mCurrentStopWatch = &mInternalStopWatch;
        mCurrentStopWatch->reset();
        mCurrentStopWatch->start();
    }
    mCurrentStopWatch->setTimeout(nGoal);

    if ( MooseUtils::scrobblableStatus( mLastSubmission ) == MooseEnums::OkToScrobble )
    {
        mbScrobblable = true;

        connect( mCurrentStopWatch, SIGNAL( timeoutReached() ),
                 this,              SLOT  ( onScrobbleTimeout() ),
                 Qt::DirectConnection );
    }
    else
    {
        mbScrobblable = false;
    }

    meState = ePlaying;
    mbScrobblePointReached = false;
}

/******************************************************************************
    OnStop
******************************************************************************/
void
CPlayerConnection::OnStop( const CPlayerCommand &cmd )
{
    // Is it OK to accept Stop at all times? Probably.

    Q_UNUSED( cmd );

    // This will block until timer thread is finished, so we need no mutex.
    // Once stop returns, the stopwatch thread is no more.
    mCurrentStopWatch->stop();
    mCurrentStopWatch->reset();

    disconnect( mCurrentStopWatch, SIGNAL( timeoutReached() ),
                this,              SLOT  ( onScrobbleTimeout() ) );

    meState = eStopped;

    ScrobbleLastSubmission();
}

/******************************************************************************
    OnPause
******************************************************************************/
void
CPlayerConnection::OnPause( const CPlayerCommand &cmd )
{
    Q_UNUSED( cmd )

    // Is it OK to accept pause at all times? Probably.
    mCurrentStopWatch->stop();

    meState = ePaused;
}

/******************************************************************************
    OnResume
******************************************************************************/
void
CPlayerConnection::OnResume( const CPlayerCommand &cmd )
{
    Q_UNUSED( cmd )

    if ( meState == eStopped || MooseUtils::scrobblableStatus( mLastSubmission ) != MooseEnums::OkToScrobble )
    {
        QString err = QT_TR_NOOP("RESUME received without a preceding START");
        LOG(2, err << "\n");
        throw BadCommandException((err));
    }

    // We allow resuming for tracks that are already playing, so we'd better
    // guard this bit.
    QMutexLocker locker( &m_mutex );

    // Only start timer off again if we haven't already submitted
    if ( mbScrobblable && !mbScrobblePointReached )
    {
        mCurrentStopWatch->start();
    }

    meState = ePlaying;
}

/******************************************************************************
    ScrobbleTimeout
******************************************************************************/
void
CPlayerConnection::onScrobbleTimeout()
{
    // Now in StopWatch thread
    QMutexLocker locker( &m_mutex );

    mbScrobblePointReached = true;

    if (The::settings().currentUser().isLogToProfile())
    {
        // User might have changed since OnStart so we set the user again here
        mLastSubmission.setUsername( The::currentUsername() );
        mLastSubmission.setPlayCount( 1 );
        mLastSubmission.setRatingFlag( TrackInfo::Scrobbled );
    
        qDebug() << mLastSubmission.username();
    
        mbReadyForSubmission = true;
        emit scrobblePointReached( mLastSubmission );

        LOG( 3, "Scrobblepoint reached for " << mLastSubmission.toString() << "\n" );
    }
    else
    {
        LOG( 3, "Scrobblepoint reached. Scrobbling disabled, will not submit.\n" );
    }
}

/******************************************************************************
    ban
******************************************************************************/
void
CPlayerConnection::ban()
{
    QMutexLocker locker( &m_mutex );

    mbReadyForSubmission = false;

    LOGL( 3, "User pressed ban, track will not be scrobbled." );
}

/******************************************************************************
    ScrobbleLastSubmission
******************************************************************************/
void
CPlayerConnection::ScrobbleLastSubmission()
{
    if ( mbReadyForSubmission )
    {
        emit submissionReady( mLastSubmission );
    }
    
    mbReadyForSubmission = false;
}

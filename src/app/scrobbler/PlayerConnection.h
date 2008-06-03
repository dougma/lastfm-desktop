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

#ifndef PLAYERCONNECTION_H
#define PLAYERCONNECTION_H

#include "PlayerCommands.h"
#include "StopWatch.h"

#include "lib/moose/TrackInfo.h"

#include <string>
#include <vector>

/*************************************************************************/ /**
    Represents a player plugin connection to the service. Keeps track of
    state and keeps a timer of how long songs have been playing. Acts as the
    command handler for commands sent by the plugin and parsed by the parser.
******************************************************************************/
class CPlayerConnection : public QObject
{
    Q_OBJECT

public:

    /*********************************************************************/ /**
        Ctors

        @param[in] sID The plugin ID for this connection
    **************************************************************************/
    CPlayerConnection(
        const QString& sID = "");
    ~CPlayerConnection();

    /*********************************************************************/ /**
        Called when a new track starts playing.

        @param[in] cmd The player command with the metadata.
    **************************************************************************/
    void
    OnStart(
        const CPlayerCommand& cmd );

    /*********************************************************************/ /**
        Called when a track is stopped or finished.
    **************************************************************************/
    void
    OnStop(
        const CPlayerCommand& cmd);

    /*********************************************************************/ /**
        Called when a track is paused.
    **************************************************************************/
    void
    OnPause(
        const CPlayerCommand& cmd);

    /*********************************************************************/ /**
        Called when a paused track starts playing again.
    **************************************************************************/
    void
    OnResume(
        const CPlayerCommand& cmd);

    /**************************************************************************
        Ban was pressed. Don't scrobble.
    **************************************************************************/
    void
    ban();

    /**************************************************************************
        Getters
    **************************************************************************/
    TrackInfo&
    GetLastSubmission() { return mLastSubmission; }

    const QString&
    GetID() { return msID; }

    StopWatch&
    GetStopWatch() { return *mCurrentStopWatch; }

    bool
    IsPlaying() { return meState == ePlaying; }

    bool
    IsPaused() { return meState == ePaused; }

    bool
    IsStopped() { return meState == eStopped; }

    bool
    IsScrobbled() { return mbReadyForSubmission; }

    bool
    IsTrackValid() { return mbScrobblable; }


public slots:

    /**************************************************************************
        Called by our owned StopWatch when a track has been listened to long
        enough to be submitted.
    **************************************************************************/
    void
    onScrobbleTimeout();

signals:

    void
    submissionReady( TrackInfo track );

    void
    scrobblePointReached( TrackInfo track );

private:

    enum EPlayerState
    {
        eStopped,
        ePlaying,
        ePaused
    };

    /*********************************************************************/ /**
        Actually send track to submitter. Happens at end of track.
    **************************************************************************/
    void
    ScrobbleLastSubmission();
    
    /*********************************************************************/ /**
        Used by copy ctor and operator=
    **************************************************************************/
    void
    Clone(
        const CPlayerConnection& that);

    EPlayerState   meState;

    QString        msID;

    TrackInfo      mLastSubmission;

    StopWatch      mInternalStopWatch;
    StopWatch*     mCurrentStopWatch;

    QMutex         m_mutex;

    bool           mbScrobblePointReached;
    bool           mbReadyForSubmission;
    bool           mbScrobblable;

};

#endif // PLAYERCONNECTION_H

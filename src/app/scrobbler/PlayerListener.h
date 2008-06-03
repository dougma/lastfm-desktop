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

#ifndef PLAYERLISTENER_H
#define PLAYERLISTENER_H

#include "PlayerCommandParser.h"
#include "PlayerConnection.h"

#include "lib/moose/TrackInfo.h"

#include <QStack>

#ifdef WIN32
    // Undef this as it's defined inside Winsock2.h
    #undef _WINSOCKAPI_
    #include <Winsock2.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <errno.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif

#ifndef WIN32
    typedef int SOCKET;

    // has to be read _and_ write for unix systems,
    // otherwise you won't be able to close and restart
    // an application instantly. instead you'd have to
    // wait for a timeout before you could bind the
    // socket to the same port again.
    #define SD_SEND SHUT_RDWR

    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define WSAENETDOWN 50
    #define WSAEMFILE 24
    #define WSAEADDRINUSE 48
    #define WSAENOBUFS 55
    #define WSAEMFILE 24
    #define WSAENOTCONN 57
    #define WSAENOTCONN2 107
    #define WSAECONNABORTED 53
    #define WSAETIMEDOUT 60
    #define WSAECONNRESET 54
    #define WSAEHOSTUNREACH 65
    #define WSAEFAULT 14
#endif

#include <map>
#include <vector>

/*************************************************************************/ /**
    The player listener is responsible for listening on a port and receive
    submissions from the player plugin about songs starting and stopping etc.

    a=<artist>&t=<track>&b=<album>&m=<mbid>&l=<length>&i=<time>&p=<path>

******************************************************************************/
class CPlayerListener : public QThread
{
    Q_OBJECT

public:

    /*********************************************************************/ /**
        Ctor
    **************************************************************************/
    CPlayerListener( QObject *parent = 0 );

    /*********************************************************************/ /**
        Returns the submission object for the last played track. If nothing's
        playing, the object will be empty.
    **************************************************************************/
    const TrackInfo&
    GetNowPlaying();

    /*********************************************************************/ /**
        Returns a pointer to the currently playing player connection object.
        Will be NULL if no player has contacted us.
    **************************************************************************/
    CPlayerConnection*
    GetActivePlayer();

    /*********************************************************************/ /**
        Returns true if any of the active players are playing.
    **************************************************************************/
    bool
    IsAnyPlaying();

    /*********************************************************************/ /**
        Stops the listener.
    **************************************************************************/
    void
    Stop();

    /*********************************************************************/ /**
        What port are we listening on?
    **************************************************************************/
    int
    GetPort() const { return mnActualPort; }

    /*********************************************************************/ /**
        QThread run method.
    **************************************************************************/
    virtual void
    run();

public slots:

    /*********************************************************************/ /**
        Work out what player connection command should go to and dish it out.
        This is called by the socket server once a player command has been
        parsed, and is also the entry point to the scrobbling system for tracks
        played on the radio.
    **************************************************************************/
    void
    Handle(
        CPlayerCommand& cmd );

    /*********************************************************************/ /**
        All player connections calls this when they reach their scrobble
        point. We arbitrate so that only the active one gets to propagate
        its signal to the Container.
    **************************************************************************/
    void
    scrobblePointReached( TrackInfo track );

signals:

    /*********************************************************************/ /**
        Emitted when the currently listened to track changes.
    **************************************************************************/
    void
    trackChanged(
        const TrackInfo& track, bool started = true ); // ugly hack

    /*********************************************************************/ /**
        Emitted when the active player scrobbles a track (i.e. when it reaches
        its scrobble point).
    **************************************************************************/
    void
    trackScrobbled(
        const TrackInfo& track );

    /*********************************************************************/ /**
        Emitted when the active player has generated bootstrapping information.
    **************************************************************************/
    void
    bootStrapping( QString userName, QString pluginId );

    /*********************************************************************/ /**
        Can't display error message boxes in worker thread so when a fatal
        error occurs, we emit this signal so the GUI thread can display
        the error.
    **************************************************************************/
    void
    exceptionThrown(
        QString msg );

private:

    /*********************************************************************/ /**
        Opens socket and waits for connections.
    **************************************************************************/
    void
    OpenSocket();

    /*********************************************************************/ /**
        Starts the listener. This will enter an infinite loop until Stop is
        called.
    **************************************************************************/
    void
    RunListener();

    /*********************************************************************/ /**
        Accepts a connecting program.
    **************************************************************************/
    void
    AcceptConnection(
        QString& sClientIPOut );

    /*********************************************************************/ /**
        Reads one line of input from the socket and stores it in sLine.
    **************************************************************************/
    void
    ReceiveLine(
        SOCKET   sock,
        QString& sLine );

    /*********************************************************************/ /**
        Sends the string down the socket.
    **************************************************************************/
    void
    SendLine(
        SOCKET             sock,
        const QString&     sResp );

    /*********************************************************************/ /**
        Send back a response indicating a bad request.
    **************************************************************************/
    void
    SendErrorResponse(
        const QString& sErrorDesc = "" );

    /*********************************************************************/ /**
        Terminate a connection.
    **************************************************************************/
    void
    ShutDownConnection(
        SOCKET sd );

    /*********************************************************************/ /**
        Wrapper around the platform-specific close calls.
    **************************************************************************/
    int
    CloseSocket(
        SOCKET sd );

    /*********************************************************************/ /**
        Wrapper around the platform-specific error code calls.
    **************************************************************************/
    int
    LastError();

    /*********************************************************************/ /**
        Search through active players for the given ID. Returns a new
        player connection if not found.
    **************************************************************************/
    CPlayerConnection&
    FindPlayerConnection(
        const QString& sID );

    /*********************************************************************/ /**
        Move the given plugin to the top of the stack.
    **************************************************************************/
    void
    StackBump(
        const QString& pluginId );

    /*********************************************************************/ /**
        Remove the given plugin from the stack.
    **************************************************************************/
    void
    StackRemove(
        const QString& pluginId );

    // The port to listen on
    static const int         DEFAULT_PORT = 33367;
    static const int         PORTS_TO_STEP = 5;         // must be same as in client
    static const int         READ_BUFFER_SIZE = 1024;   // bytes
    static const int         RECV_TIMEOUT = 5000;       // timeout on recv (ms)
    static const QString     HOST_ADDRESS;              // localhost

    CPlayerCommandParser mParser;

    SOCKET      mListeningSocket;
    SOCKET      mTalkSocket;

    // Maps player IDs to player connection objects. Players are added to this
    // as soon as a command is received from them and they are never deleted
    // during the lifetime of the listener.
    std::map<QString, CPlayerConnection*> mActivePlayers;

    // Most recently submitted from player is top of the stack. Used to manage
    // what track to return from GetNowPlaying.
    QStack<QString> mPlayerStack;
    const TrackInfo mEmptyTrack;

    std::map<EPlayerResponse, QString>  mResponseMap;

    int         mnActualPort; // stores the port used in case of port stepping
    bool        mbRunning;
    bool        mbKeepGoing;

    bool        mRadioIsPlaying;
};

#endif // PLAYERLISTENER_H

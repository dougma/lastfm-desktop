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
#include "PlayerListener.h"
#include "PlayerConnection.h"

#include "lib/unicorn/Logger.h"
#include "lib/moose/MooseCommon.h"

#include <sstream>
#include <iostream>
#include <algorithm>

using namespace std;


/******************************************************************************
    CPlayerListener
******************************************************************************/
CPlayerListener::CPlayerListener( QObject* parent ) :
        QThread( parent ),
        mListeningSocket(INVALID_SOCKET),
        mTalkSocket(INVALID_SOCKET),
        mbRunning(false),
        mbKeepGoing(true),
        mRadioIsPlaying(false)
{
    // Initialise the response map
    mResponseMap[PRESP_OK]      = "OK";
    mResponseMap[PRESP_ERROR]   = "ERROR";
}


/******************************************************************************
    GetNowPlaying
******************************************************************************/
const TrackInfo&
CPlayerListener::GetNowPlaying()
{
    // TODO: scary race conditions here when called from the GUI thread

    if (!mPlayerStack.isEmpty())
    {
        QString id = mPlayerStack.top();
        Q_ASSERT(mActivePlayers.find(id) != mActivePlayers.end());
        CPlayerConnection* player = mActivePlayers[id];
        return player->GetLastSubmission();
    }
    else
    {
        return mEmptyTrack;
    }
}


/******************************************************************************
    GetActivePlayer
******************************************************************************/
CPlayerConnection*
CPlayerListener::GetActivePlayer()
{
    // TODO: scary race conditions here when called from the GUI thread

    if (!mPlayerStack.isEmpty())
    {
        QString id = mPlayerStack.top();
        Q_ASSERT(mActivePlayers.find(id) != mActivePlayers.end());
        return mActivePlayers[id];
    }
    else
    {
        return NULL;
    }
}


/******************************************************************************
    run, this is the thread entry procedure.
******************************************************************************/
void
CPlayerListener::run()
{
    try
    {
        RunListener();
    }
    catch (LastFmException& e)
    {
        emit exceptionThrown(e.tr_what());
    }
}


/******************************************************************************
    RunListener
******************************************************************************/
void
CPlayerListener::RunListener()
{
    try
    {
        OpenSocket();
    }
    catch (NetworkException& e)
    {
        QString err = tr(
            "Last.fm was unable to connect to the plugin for your media player.\n\n"
            "Details: %1" ).arg( e.what() );
        LOGL( 1, err );
        throw NetworkException((err));
    }

    mbRunning = true;

    while (mbKeepGoing)
    {
        QString sClientIP;
        try
        {
            AcceptConnection( sClientIP );
        }
        catch (NetworkException& e)
        {
            // The listener's not gonna work if this throws so let it propagate up
            QString err = tr(
                "Last.fm was unable to start accepting tracks from the plugin for your media player.\n\n"
                "Details: %1").arg( e.what() );
            LOG(1, err << "\n");
            throw NetworkException((err));
        }

        try
        {
            QString sCmd;

            // Throws CNetworkException or CBadCommandException
            ReceiveLine(mTalkSocket, sCmd);

            // Throws CParseException
            CPlayerCommand cmd;
            mParser.Parse(sCmd.toStdString(), cmd);

            // Throws CBadCommandException
            Handle(cmd);

            // Command handled successfully, respond. Throws CNetworkException.
            SendLine(mTalkSocket, mResponseMap[PRESP_OK]);

        }
        catch (const NetworkException& e)
        {
            // We probably don't have a socket to send response through here,
            // so don't. Just break out to ShutDownConnection and loop back up.
            LOG(1, "NetworkException: " << e.what() << "\n");
        }
        catch (const BadClientException& e)
        {
            LOG(1, "BadClientException: " << e.what() << "\n");
            SendErrorResponse(e.what());
        }
        catch (const ParseException& e)
        {
            LOG(1, "ParseException: " << e.what() << "\n");
            SendErrorResponse(e.what());
        }
        catch (const BadCommandException& e)
        {
            LOG(1, "BadCommandException: " << e.what() << "\n");
            SendErrorResponse(e.what());
        }

        // Doesn't throw
        ShutDownConnection(mTalkSocket);

    } // end while keep going

    LOG(3, "CPlayerListener shutting down thread\n");

    ShutDownConnection(mListeningSocket);

    #ifdef WIN32
        WSACleanup();
    #endif

}


/******************************************************************************
    Stop
******************************************************************************/
void
CPlayerListener::Stop()
{
    Q_DEBUG_BLOCK;

    // This will be called by the main GUI thread. What we'll do to shut
    // down is simply post an EXIT message to ourselves to shut down
    // gracefully.
    if (!mbRunning) { return; }

    QString sExit = "EXIT c=tst";
    SOCKET sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd != INVALID_SOCKET)
    {
//         u_long nRemoteAddr = inet_addr(qPrintable(HOST_ADDRESS));
        u_long nRemoteAddr = htonl( INADDR_LOOPBACK );
        sockaddr_in sinRemote;
        sinRemote.sin_family = AF_INET;
        sinRemote.sin_addr.s_addr = nRemoteAddr;
        sinRemote.sin_port = htons(mnActualPort);
        if (::connect(sd, (sockaddr*)&sinRemote, sizeof(sockaddr_in)) == SOCKET_ERROR)
        {
            sd = INVALID_SOCKET;
            int nErrorCode = LastError();
            LOG(1, "Couldn't connect to listener to send Stop. Socket error: " << nErrorCode << "\n");
            Q_ASSERT(false);
        }

        try
        {
            SendLine(sd, sExit);
            ReceiveLine(sd, sExit);
            ShutDownConnection(sd);
        }
        catch (LastFmException& e)
        {
            // Don't want this to propagate as we're shutting down.
            int nErrorCode = LastError();
            LOG(1, "Exception when sending Stop: " << e.what() << "\n");

            Q_UNUSED( nErrorCode );
        }
    }

    // Send stop messages to all active players to shut down their timer threads
    map<QString, CPlayerConnection*>::iterator pos;
    for (pos = mActivePlayers.begin(); pos != mActivePlayers.end(); ++pos)
    {
        CPlayerCommand dummy;
        pos->second->OnStop(dummy);
    }

    qDebug() << "Waiting on thread...";
    wait( 5000 );

    for (pos = mActivePlayers.begin(); pos != mActivePlayers.end(); ++pos)
    {
        delete pos->second;
    }
}


/******************************************************************************
    OpenSocket
******************************************************************************/
void
CPlayerListener::OpenSocket()
{
    #ifdef WIN32
        WSADATA wsaData;
        memset(&wsaData, 0, sizeof(WSADATA));
        int ret = WSAStartup(MAKEWORD(1, 0), &wsaData);
        LOG(3, wsaData.szDescription << " " << wsaData.szSystemStatus << ". " <<
            "wVersion: " << (int)LOBYTE(wsaData.wVersion) << "." << (int)HIBYTE(wsaData.wVersion) << ", " <<
            "wHighVersion: " << (int)LOBYTE(wsaData.wHighVersion) << "." << (int)HIBYTE(wsaData.wHighVersion) << "\n");
        if (ret != 0)
        {
            // Can't call WSAGetLastError as Winsock hasn't been initialised
            QString err = tr("Initialisation of Winsock failed. WSAStartup error code: %1").arg( ret );
            WSACleanup();
            throw NetworkException((err));
        }
    #endif

    // Get a socket handle
    mListeningSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Possible failures:
    // WSANOTINITIALISED A successful WSAStartup call must occur before using this function. 
    // WSAENETDOWN The network subsystem or the associated service provider has failed. 
    // WSAEAFNOSUPPORT The specified address family is not supported. 
    // WSAEINPROGRESS A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. 
    // WSAEMFILE No more socket descriptors are available. 
    // WSAENOBUFS No buffer space is available. The socket cannot be created. 
    // WSAEPROTONOSUPPORT The specified protocol is not supported. 
    // WSAEPROTOTYPE The specified protocol is the wrong type for this socket. 
    // WSAESOCKTNOSUPPORT The specified socket type is not supported in this address family.
    // All programmer errors, assert.
    if (mListeningSocket == INVALID_SOCKET)
    {
        int nErrorCode = LastError();
        QString err = tr("Could not create listening socket. Socket error: %1").arg( nErrorCode );
        if (nErrorCode == WSAENETDOWN ||
            nErrorCode == WSAEMFILE ||
            nErrorCode == WSAENOBUFS)
        {
            // Unforeseen errors
            throw NetworkException((err));
        }
        else
        {
            // Programmer error
            Q_ASSERT(false);
            throw NetworkException((err));
        }
    }

    // Bind socket to localhost
    struct sockaddr_in sinInterface;
    // Zero the entire sinInterface. Important, otherwise it
    // results in undefined behaviour when building a release
    memset( &sinInterface, '\0', sizeof( sinInterface ) );

    sinInterface.sin_family = AF_INET;
    sinInterface.sin_addr.s_addr = htonl( INADDR_LOOPBACK );

    // htons converts a short to TCP/IP byte order (big-endian)
    mnActualPort = DEFAULT_PORT;
    sinInterface.sin_port = htons(mnActualPort);

#ifndef WIN32
    {
        int on = 1;
        setsockopt( mListeningSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof( on ) );
    }
#endif

    int nResult = bind( mListeningSocket, (struct sockaddr *)&sinInterface, sizeof( sinInterface ) );

    // Possible failures:
    // WSANOTINITIALISED A successful WSAStartup call must occur before using this function. 
    // WSAENETDOWN The network subsystem has failed. 
    // WSAEACCES Attempt to connect datagram socket to broadcast address failed because setsockopt option SO_BROADCAST is not enabled. 
    // WSAEADDRINUSE A process on the computer is already bound to the same fully-qualified address and the socket has not been marked to allow address reuse with SO_REUSEADDR. For example, the IP address and port are bound in the af_inet case). (See the SO_REUSEADDR socket option under setsockopt.) 
    // WSAEADDRNOTAVAIL The specified address is not a valid address for this computer. 
    // WSAEFAULT The name or namelen parameter is not a valid part of the user address space, the namelen parameter is too small, the name parameter contains an incorrect address format for the associated address family, or the first two bytes of the memory block specified by name does not match the address family associated with the socket descriptor s. 
    // WSAEINPROGRESS A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. 
    // WSAEINVAL The socket is already bound to an address. 
    // WSAENOBUFS Not enough buffers available, too many connections. 
    // WSAENOTSOCK The descriptor is not a socket. 
    // Address in use could potentially be a problem here, maybe use a port stepping scheme.
    if (nResult == SOCKET_ERROR)
    {
        int nErrorCode = LastError();
        QString err = tr("Could not bind listening socket. Socket error: %1").arg( nErrorCode );
        if (nErrorCode == WSAEADDRINUSE)
        {
            // Do port stepping
            int i;
            for (i = 1; i <= PORTS_TO_STEP; ++i)
            {
                sinInterface.sin_port = htons(++mnActualPort);
                nResult = bind(mListeningSocket,
                    reinterpret_cast<sockaddr*>(&sinInterface),
                    sizeof(sockaddr_in));
                if (nResult == SOCKET_ERROR)
                {
                    nErrorCode = LastError();
                    err = tr("Could not bind listening socket. Socket error: %1").arg( nErrorCode );
                }
                else
                {
                    nErrorCode = LastError();
                    break;
                }
            }

            if (i > PORTS_TO_STEP)
            {
                LOG(1, "Ran " << PORTS_TO_STEP << " iterations without finding a good port\n");
                CloseSocket(mListeningSocket);
                throw NetworkException((err));
            }
        }

        // This is to catch error conditions other than WSAEADDRINUSE that
        // could have resulted from the port stepping loop
        if (nResult == SOCKET_ERROR)
        {
            if (nErrorCode == WSAENETDOWN ||
                nErrorCode == WSAENOBUFS)
            {
                // Unforeseen errors
                CloseSocket(mListeningSocket);
                throw NetworkException((err));
            }
            else
            {
                // Programmer error
                CloseSocket(mListeningSocket);
                throw NetworkException((err));
            }
        }
    }

    LOG(3, "Listener bound to port " << mnActualPort << "\n");

    nResult = listen(mListeningSocket, 1);

    // Possible failures:
    // WSANOTINITIALISED A successful WSAStartup call must occur before using this function. 
    // WSAENETDOWN The network subsystem has failed. 
    // WSAEADDRINUSE The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR. This error usually occurs during execution of the bind function, but could be delayed until this function if the bind was to a partially wildcard address (involving ADDR_ANY) and if a specific address needs to be committed at the time of this function. 
    // WSAEINPROGRESS A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. 
    // WSAEINVAL The socket has not been bound with bind. 
    // WSAEISCONN The socket is already connected. 
    // WSAEMFILE No more socket descriptors are available. 
    // WSAENOBUFS No buffer space is available. 
    // WSAENOTSOCK The descriptor is not a socket. 
    // WSAEOPNOTSUPP The referenced socket is not of a type that supports the listen operation. 
    // If this fails, just throw an exception. The ADDRINUSE error won't happen
    // for us, it will always happen on bind.
    if (nResult == SOCKET_ERROR)
    {
        int nErrorCode = LastError();
        QString err = tr("Could not start listening on socket. Socket error: %1").arg( nErrorCode );
        if (nErrorCode == WSAENETDOWN ||
            nErrorCode == WSAEMFILE ||
            nErrorCode == WSAENOBUFS)
        {
            // Unforeseen errors
            CloseSocket(mListeningSocket);
            throw NetworkException((err));
        }
        else
        {
            // Programmer error
            Q_ASSERT(false);
            CloseSocket(mListeningSocket);
            throw NetworkException((err));
        }
    }
}


/******************************************************************************
    AcceptConnection
******************************************************************************/
void
CPlayerListener::AcceptConnection(
    QString& sClientIPOut)
{
    sockaddr_in clientAddr;
    #ifdef WIN32
    int nAddrSize = sizeof(clientAddr);
    #else
    unsigned int nAddrSize = sizeof(clientAddr);
    #endif

    Q_ASSERT(mListeningSocket != INVALID_SOCKET);

    // Wait for a connection, this blocks
    mTalkSocket = accept(mListeningSocket,
                         reinterpret_cast<sockaddr*>(&clientAddr),
                         &nAddrSize);

    // Possible failures:
    // WSANOTINITIALISED A successful WSAStartup call must occur before using this function. 
    // WSAENETDOWN The network subsystem has failed. 
    // WSAEFAULT The addrlen parameter is too small or addr is not a valid part of the user address space. 
    // WSAEINTR A blocking Windows Sockets 1.1 call was canceled through WSACancelBlockingCall. 
    // WSAEINPROGRESS A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. 
    // WSAEINVAL The listen function was not invoked prior to accept. 
    // WSAEMFILE The queue is nonempty upon entry to accept and there are no descriptors available. Run out of sockets.
    // WSAENOBUFS No buffer space is available. 
    // WSAENOTSOCK The descriptor is not a socket. 
    // WSAEOPNOTSUPP The referenced socket is not a type that supports connection-oriented service. 
    // WSAEWOULDBLOCK The socket is marked as nonblocking and no connections are present to be accepted. 
    if (mTalkSocket == INVALID_SOCKET)
    {
        // A failure here will always happen on entry to accept, not after blocking.
        // I think. So we'll just throw on one of the unforeseen ones.
        int nErrorCode = LastError();
        QString err = tr("Accept socket connection failed. Socket error: %1").arg( nErrorCode );
        if (nErrorCode == WSAENETDOWN ||
            nErrorCode == WSAEMFILE ||
            nErrorCode == WSAENOBUFS ||
            nErrorCode == WSAEFAULT)
        {
            // Unforeseen errors, close and throw
            CloseSocket(mListeningSocket);
            throw NetworkException((err));
        }
        else
        {
            // Programmer error
            Q_ASSERT(false);
            CloseSocket(mListeningSocket);
            throw NetworkException((err));
        }
    }

    // needed for non-windows
    #ifndef WIN32
    struct timeval nTimeout;
    nTimeout.tv_sec = 0;
    nTimeout.tv_usec = RECV_TIMEOUT;
    #endif

    #ifdef WIN32
    int nTimeout = RECV_TIMEOUT;
    #endif

    // Set timeout on socket. (Requires WinSock2 on Win-systems)
    int nResult = setsockopt(
        mTalkSocket,
        SOL_SOCKET, // level
        SO_RCVTIMEO, // option
    #ifdef WIN32
        (const char*)&nTimeout,
    #endif
    #ifndef WIN32
        &nTimeout,
    #endif
        sizeof( nTimeout ) );

    if (nResult == SOCKET_ERROR)
    {
        int nErrorCode = LastError();
        LOG(1, "Set socket timeout failed. Socket error: " << nErrorCode << "\n");
        Q_ASSERT(false);
    }

    // Convert the IP address into the passed in string
    char* pcClientIP = inet_ntoa(clientAddr.sin_addr);
    sClientIPOut = pcClientIP;
}


/******************************************************************************
    ReceiveLine
******************************************************************************/
void
CPlayerListener::ReceiveLine(
    SOCKET  sock,
    QString& sLine)
{
    Q_ASSERT(sock != INVALID_SOCKET);

    // Read data from client. This data is UTF-8.
    char acReadBuffer[READ_BUFFER_SIZE];
    int nReadSize = READ_BUFFER_SIZE - 1;   // so that we can append a \0
    int nBytesRead;

    // Go into loop that keeps going until a CR is received
    bool bKeepGoing = true;
    do
    {
        // Will block until there is data available or timeout is reached
        nBytesRead = recv(sock, acReadBuffer, nReadSize, 0);

        // Possible failures:
        // WSANOTINITIALISED A successful WSAStartup call must occur before using this function. 
        // WSAENETDOWN The network subsystem has failed. 
        // WSAEFAULT The buf parameter is not completely contained in a valid part of the user address space. 
        // WSAENOTCONN The socket is not connected. 
        // WSAEINTR The (blocking) call was canceled through WSACancelBlockingCall. 
        // WSAEINPROGRESS A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. 
        // WSAENETRESET The connection has been broken due to the keep-alive activity detecting a failure while the operation was in progress. 
        // WSAENOTSOCK The descriptor is not a socket. 
        // WSAEOPNOTSUPP MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only send operations. 
        // WSAESHUTDOWN The socket has been shut down; it is not possible to receive on a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH. 
        // WSAEWOULDBLOCK The socket is marked as nonblocking and the receive operation would block. 
        // WSAEMSGSIZE The message was too large to fit into the specified buffer and was truncated. 
        // WSAEINVAL The socket has not been bound with bind, or an unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled or (for byte stream sockets only) len was zero or negative. 
        // WSAECONNABORTED The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable. 
        // WSAETIMEDOUT The connection has been dropped because of a network failure or because the peer system failed to respond. 
        // WSAECONNRESET The virtual circuit was reset by the remote side executing a hard or abortive close. The application should close the socket as it is no longer usable. On a UPD-datagram socket this error would indicate that a previous send operation resulted in an ICMP "Port Unreachable" message. 
        if (nBytesRead == SOCKET_ERROR || nBytesRead == 0)
        {
            int nErrorCode = LastError();
            QString err = tr("Receiving data through socket failed. Socket error: %1").arg( nErrorCode );
            if (nErrorCode == WSAENETDOWN ||
                nErrorCode == WSAENOTCONN ||
                nErrorCode == WSAECONNABORTED ||
                nErrorCode == WSAETIMEDOUT ||
                nErrorCode == WSAECONNRESET || // client app killed
                nBytesRead == 0) // graceful client shutdown
            {
                // This happens when something goes wrong, client dies or something.
                // Caller should close talk socket and recover.
                throw NetworkException((err));
            }
            else
            {
                // Programmer error
                #ifdef WIN32
                Q_ASSERT(false);
                throw NetworkException((err));
                #endif
            }
        }
        else
        {
            // Read successful, is last char a CR?
            if (acReadBuffer[nBytesRead - 1] == '\n')
            {
                // Step back to before \n
                nBytesRead--;

                // On Windows, CR is "\r\n", that's what we get if using
                // Telnet so then 2 bytes would need to be removed. If called
                // from C++ though, a single "\n" is always sent.
                // TODO: Mac?
                if (acReadBuffer[nBytesRead - 1] == '\r')
                {
                    nBytesRead--;
                }

                // If so, we're done
                bKeepGoing = false;
            }

            acReadBuffer[nBytesRead] = '\0';

            sLine += acReadBuffer;

        } // end nBytesRead

    } while (bKeepGoing);

    LOG(3, "Line received: " << sLine << "\n");

}


/******************************************************************************
    SendLine
******************************************************************************/
void
CPlayerListener::SendLine(
    SOCKET         sock,
    const QString& sResp)
{
    Q_ASSERT(sock != INVALID_SOCKET);

    // Add CR
    QString sFormattedResp = sResp + "\n";

    int nBytesToSend = static_cast<int>(sFormattedResp.size());
    int nTotalBytesSent = 0;

    LOG(4, "Sending: " << sResp << "\n");

    while (nTotalBytesSent < nBytesToSend)
    {
        // Should never block
        int nBytesSent = send(sock,
                              qPrintable(sFormattedResp),
                              nBytesToSend - nTotalBytesSent,
                              0);

        // Possible failures:
        // WSANOTINITIALISED A successful WSAStartup call must occur before using this function. 
        // WSAENETDOWN The network subsystem has failed. 
        // WSAEACCES The requested address is a broadcast address, but the appropriate flag was not set. Call setsockopt with the SO_BROADCAST parameter to allow the use of the broadcast address. 
        // WSAEINTR A blocking Windows Sockets 1.1 call was canceled through WSACancelBlockingCall. 
        // WSAEINPROGRESS A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. 
        // WSAEFAULT The buf parameter is not completely contained in a valid part of the user address space. 
        // WSAENETRESET The connection has been broken due to the keep-alive activity detecting a failure while the operation was in progress. 
        // WSAENOBUFS No buffer space is available. 
        // WSAENOTCONN The socket is not connected. 
        // WSAENOTSOCK The descriptor is not a socket. 
        // WSAEOPNOTSUPP MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only receive operations. 
        // WSAESHUTDOWN The socket has been shut down; it is not possible to send on a socket after shutdown has been invoked with how set to SD_SEND or SD_BOTH. 
        // WSAEWOULDBLOCK The socket is marked as nonblocking and the requested operation would block. 
        // WSAEMSGSIZE The socket is message oriented, and the message is larger than the maximum supported by the underlying transport. 
        // WSAEHOSTUNREACH The remote host cannot be reached from this host at this time. 
        // WSAEINVAL The socket has not been bound with bind, or an unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled. 
        // WSAECONNABORTED The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable. 
        // WSAECONNRESET The virtual circuit was reset by the remote side executing a hard or abortive close. For UPD sockets, the remote host was unable to deliver a previously sent UDP datagram and responded with a "Port Unreachable" ICMP packet. The application should close the socket as it is no longer usable. 
        // WSAETIMEDOUT The connection has been dropped, because of a network failure or because the system on the other end went down without notice. 
        if (nBytesSent == SOCKET_ERROR || nBytesSent == 0)
        {
            int nErrorCode = LastError();
            QString err = tr("Sending of data through socket failed. Socket error: %1").arg( nErrorCode );
            if (nErrorCode == WSAENETDOWN ||
                nErrorCode == WSAENOBUFS ||
                nErrorCode == WSAEHOSTUNREACH ||
                nErrorCode == WSAECONNABORTED ||
                nErrorCode == WSAECONNRESET ||
                nErrorCode == WSAETIMEDOUT ||
                nBytesSent == 0)
            {
                // Something unforeseen went wrong, so throw and let caller abort
                throw NetworkException((err));
            }
            else
            {
                // Programmer error
                Q_ASSERT(false);
                throw NetworkException((err));
            }
        }
        else
        {
            nTotalBytesSent += nBytesSent;
        }

    } // end while
}


/******************************************************************************
    Handle
******************************************************************************/
void
CPlayerListener::Handle( CPlayerCommand& cmd )
{
    // HACK: check for exit command before user check as we could get an exit
    // before we have a current user.
    if ( cmd.mCmd == PCMD_EXIT )
    {
        mbKeepGoing = false;
        return;
    }

    // Earlier checks should have ensured we always have a plugin ID here.
    Q_ASSERT( !cmd.mPluginId.isEmpty() );

    // Get corresponding player connection object
    CPlayerConnection& player = FindPlayerConnection(cmd.mPluginId);

    switch (cmd.mCmd)
    {
        case PCMD_START:
        case PCMD_RESUME:
        {
            cmd.mCmd == PCMD_START ? player.OnStart(cmd) : player.OnResume(cmd);
            StackBump( cmd.mPluginId );

            if ( cmd.mPluginId == MooseConstants::kRadioPluginId )
            {
                mRadioIsPlaying = true;
            }

            // We want radio to take priority in the GUI at all times so we
            // only emit a new track for a scrobbler if radio isn't already
            // playing
            if ( cmd.mPluginId == MooseConstants::kRadioPluginId || !mRadioIsPlaying )
            {
                emit trackChanged( GetNowPlaying(), true );
            }
        }
        break;

        case PCMD_STOP:
        case PCMD_PAUSE:
        {
            cmd.mCmd == PCMD_STOP ? player.OnStop(cmd) : player.OnPause(cmd);
            StackRemove( cmd.mPluginId );

            if ( cmd.mPluginId == MooseConstants::kRadioPluginId )
            {
                mRadioIsPlaying = false;
            }

            if ( !mRadioIsPlaying )
            {
                const TrackInfo& np = GetNowPlaying();
                bool stopped = np.isEmpty();
                emit trackChanged( np, !stopped );
            }
        }
        break;

        case PCMD_BOOTSTRAP:
            emit bootStrapping( cmd.mUserName, cmd.mPluginId );
        break;

        default:
            Q_ASSERT( !"No, this is not the type of command we like around 'ere" );

    }

}


/******************************************************************************
    scrobblePointReached
******************************************************************************/
void
CPlayerListener::scrobblePointReached( TrackInfo track )
{
    QObject* snd = sender();

    if ( snd == GetActivePlayer() )
    {
        emit trackScrobbled( track );
    }
}


/******************************************************************************
    SendErrorResponse
******************************************************************************/
void
CPlayerListener::SendErrorResponse(
    const QString& sErrorDesc)
{
    QString err =  mResponseMap[PRESP_ERROR] + ": " + sErrorDesc;

    try
    {
        SendLine(mTalkSocket, err);
    }
    catch (NetworkException& e)
    {
        // When sending an error response, we don't want exceptions
        // propagating up to the caller as the error response is invoked
        // from a catch handler.
        LOG(1, "NetworkException: " << e.what() << "\n");
    }
}


/******************************************************************************
    ShutDownConnection
******************************************************************************/
void
CPlayerListener::ShutDownConnection(
    SOCKET sd)
{
    // Disallow any further data sends.  This will tell the other side
    // that we want to go away now.  If we skip this step, we don't
    // shut the connection down nicely.
    int nResult = shutdown(sd, SD_SEND);

    // Possible failures:
    // WSANOTINITIALISED A successful WSAStartup call must occur before using this function.
    // WSAENETDOWN The network subsystem has failed.
    // WSAEINVAL The how parameter is not valid, or is not consistent with the socket type. For example, SD_SEND is used with a UNI_RECV socket type.
    // WSAEINPROGRESS A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.
    // WSAENOTCONN The socket is not connected (connection-oriented sockets only).
    // WSAENOTSOCK The descriptor is not a socket.
    if (nResult == SOCKET_ERROR)
    {
        int nErrorCode = LastError();
        LOG(1, "Socket shutdown failed. Socket error: " << nErrorCode);
        if (nErrorCode == WSAENETDOWN ||
#ifndef WIN32
            nErrorCode == WSAENOTCONN2 ||
#endif
            nErrorCode == WSAENOTCONN)
        {
            // Even though these are unforeseen failures, we don't want to
            // throw as we're shutting down. Just carry on shutting down
            // best we can.
        }
        else
        {
            // Programmer error
            Q_ASSERT(false);
        }
    }

    // Receive any extra data still sitting on the socket.  After all
    // data is received, this call will block until the remote host
    // acknowledges the TCP control packet sent by the shutdown above.
    // Then we'll get a 0 back from recv, signalling that the remote
    // host has closed its side of the connection.
    char acReadBuffer[READ_BUFFER_SIZE];
    while (true)
    {
        int nBytesRead = recv(sd, acReadBuffer, READ_BUFFER_SIZE, 0);

        // Possible failures:
        // WSANOTINITIALISED A successful WSAStartup call must occur before using this function.
        // WSAENETDOWN The network subsystem has failed.
        // WSAEFAULT The buf parameter is not completely contained in a valid part of the user address space.
        // WSAENOTCONN The socket is not connected.
        // WSAEINTR The (blocking) call was canceled through WSACancelBlockingCall.
        // WSAEINPROGRESS A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.
        // WSAENETRESET The connection has been broken due to the keep-alive activity detecting a failure while the operation was in progress.
        // WSAENOTSOCK The descriptor is not a socket.
        // WSAEOPNOTSUPP MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only send operations.
        // WSAESHUTDOWN The socket has been shut down; it is not possible to receive on a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH.
        // WSAEWOULDBLOCK The socket is marked as nonblocking and the receive operation would block.
        // WSAEMSGSIZE The message was too large to fit into the specified buffer and was truncated.
        // WSAEINVAL The socket has not been bound with bind, or an unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled or (for byte stream sockets only) len was zero or negative.
        // WSAECONNABORTED The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable.
        // WSAETIMEDOUT The connection has been dropped because of a network failure or because the peer system failed to respond.
        // WSAECONNRESET The virtual circuit was reset by the remote side executing a hard or abortive close. The application should close the socket as it is no longer usable. On a UPD-datagram socket this error would indicate that a previous send operation resulted in an ICMP "Port Unreachable" message.
        if (nBytesRead == SOCKET_ERROR)
        {
            // I don't think we care here if the call failed.
            // Just carry on shutting down best we can.
            int nErrorCode = LastError();
            LOG(1, "Waiting for shutdown confirmation failed. Socket error: " << nErrorCode << "\n");
            if (nErrorCode == WSAENETDOWN ||
                nErrorCode == WSAENOTCONN ||
#ifndef WIN32
                nErrorCode == WSAENOTCONN2 ||
#endif
                nErrorCode == WSAECONNABORTED ||
                nErrorCode == WSAETIMEDOUT ||
                nErrorCode == WSAECONNRESET || // client app killed
                nBytesRead == 0) // graceful client shutdown
            {
                // This happens when something goes wrong, client dies or something.
                // Caller should close talk socket and recover.
            }
            else
            {
                // Programmer error
                Q_ASSERT(false);
            }
            break;
        }
        else if (nBytesRead != 0)
        {
            string sTemp;
            sTemp.append(acReadBuffer, nBytesRead);
            LOG(3, "Received " << nBytesRead << " unexpected bytes during shutdown: " <<
                 sTemp << "\n");
        }
        else
        {
            // Returned 0, what we expect
            break;
        }
    }

    // Close the socket
    nResult = CloseSocket(sd);

    // Possible failures:
    // WSANOTINITIALISED A successful WSAStartup call must occur before using this function.
    // WSAENETDOWN The network subsystem has failed.
    // WSAENOTSOCK The descriptor is not a socket.
    // WSAEINPROGRESS A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.
    // WSAEINTR The (blocking) Windows Socket 1.1 call was canceled through WSACancelBlockingCall.
    // WSAEWOULDBLOCK The socket is marked as nonblocking and SO_LINGER is set to a nonzero time-out value.
    if (nResult == SOCKET_ERROR)
    {
        int nErrorCode = LastError();
        LOG(1, "Couldn't close socket. Socket error: " << nErrorCode << "\n");
        if (nErrorCode == WSAENETDOWN)
        {
            // Again, we don't want to throw. Log the error and continue.
        }
        else
        {
            // Programmer error
            Q_ASSERT(false);
        }
    }

}


/******************************************************************************
    CloseSocket
******************************************************************************/
int
CPlayerListener::CloseSocket(
    SOCKET sd)
{
#ifdef WIN32
    return closesocket(sd);
#else
    return close(sd);
#endif // WIN32
}


/******************************************************************************
    LastError
******************************************************************************/
int
CPlayerListener::LastError()
{
#ifdef WIN32
    return WSAGetLastError();
#else
    return errno;
#endif // WIN32
}


/******************************************************************************
    FindPlayerConnection
******************************************************************************/
CPlayerConnection&
CPlayerListener::FindPlayerConnection( const QString& sID )
{
    if (mActivePlayers.find(sID) != mActivePlayers.end())
    {
        return *mActivePlayers[sID];
    }

    // Didn't find it, create a new one
    CPlayerConnection* newPlayer = new CPlayerConnection(sID);
    mActivePlayers[sID] = newPlayer;

    LOGL( 3, "Creating active player: " << sID );

#if 0
    //TODO
    bool success = connect(
            mActivePlayers[sID], SIGNAL(submissionReady( TrackInfo )),
            &The::scrobbler(), SLOT(scrobble( TrackInfo )),
            Qt::QueuedConnection );

    if ( !success )
    {
        LOGL( 1, "Failed to connect new player to the scrobbler." );
        Q_ASSERT( false );
    }
#endif

    connect(
        mActivePlayers[sID],    SIGNAL( scrobblePointReached(TrackInfo) ),
        this,                   SLOT  ( scrobblePointReached(TrackInfo) ),
        Qt::QueuedConnection );

    return *mActivePlayers[sID];
}


/******************************************************************************
    IsAnyPlaying
******************************************************************************/
bool
CPlayerListener::IsAnyPlaying()
{
    map<QString, CPlayerConnection*>::iterator pos;
    for (pos = mActivePlayers.begin(); pos != mActivePlayers.end(); ++pos)
    {
        if (pos->second->IsPlaying())
        {
            return true;
        }
    }

    return false;
}


/******************************************************************************
    StackBump
******************************************************************************/
void
CPlayerListener::StackBump(
    const QString& pluginId)
{
    // Find it and move it to the top
    int idx = mPlayerStack.indexOf( pluginId );
    if ( idx != -1 )
    {
        mPlayerStack.remove( idx );
    }
    mPlayerStack.push( pluginId );

    // If the radio is playing, always put it back up at the top
    idx = mPlayerStack.indexOf( MooseConstants::kRadioPluginId );
    if ( idx != -1 )
    {
        mPlayerStack.remove( idx );
        mPlayerStack.push( MooseConstants::kRadioPluginId );

        Q_ASSERT( mPlayerStack.top() == MooseConstants::kRadioPluginId );
    }
}


/******************************************************************************
    StackRemove
******************************************************************************/
void
CPlayerListener::StackRemove(
    const QString& pluginId)
{
    int idx = mPlayerStack.indexOf(pluginId);
    if (idx != -1)
    {
        mPlayerStack.remove(idx);
    }
}

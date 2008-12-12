/* Copyright 205-2009, Last.fm Ltd. <client@last.fm>                       
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY <copyright holder> ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef BLOCKINGCLIENT_H
#define BLOCKINGCLIENT_H

#ifdef WIN32
    #include <winsock2.h>
#else
    #include <stdexcept>    //TODO: check if this /should/ be included in Win32
    #include <sys/socket.h>
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define SD_SEND SHUT_WR    
    typedef int SOCKET;
    typedef uint16_t u_short;
    typedef uint32_t u_long;
#endif

#include <string>

/*************************************************************************/ /**
    Sockets-based client for sending and receiving commands down a socket
    using Winsock2.

    @author <erik@last.fm>
******************************************************************************/
class BlockingClient
{
public:

    /*************************************************************************
        Exception class for when things go wrong.
    **************************************************************************/
    class NetworkException : public std::logic_error {
    public:
        NetworkException(const std::string& sMsg) : logic_error(sMsg) { }
    };

    /*********************************************************************/ /**
        Ctor
    **************************************************************************/
	BlockingClient();

    /*********************************************************************/ /**
        Dtor
    **************************************************************************/
	virtual
	~BlockingClient();

    /*********************************************************************/ /**
        Connect to server.
        
        @param[in] sHost The host to connect to.
        @param[in] nPort The port on the host to connect to.
    **************************************************************************/
    void
    Connect(
        const std::string& sHost,
        u_short            nPort);

    /*********************************************************************/ /**
        Send a string down the socket.

        @param[in] sData The data to send.
    **************************************************************************/
    void
    Send(
        const std::string& sData);

    /*********************************************************************/ /**
        Read a line from the socket.

        @param[out] sLine String the received line is written to.
    **************************************************************************/
    void
    Receive(
        std::string& sLine);

    /*********************************************************************/ /**
        Shut down connection gracefully.
    **************************************************************************/
    void
    ShutDown();

    /*********************************************************************/ /**
        Check if socket is connected.
        //TODO: document return..    
    **************************************************************************/

    bool
    isConnected();

private:
    
    /*********************************************************************/ /**
        Given an address string, determine if it's a dotted-quad IP address
        or a domain address. If the latter, ask DNS to resolve it. In either
        case, return resolved IP address. Throws CNetworkException on fail.
    **************************************************************************/
    u_long
    LookUpAddress(
        const std::string& sHost);

    /*********************************************************************/ /**
        Connects to a given address, on a given port, both of which must be
        in network byte order. Returns newly-connected socket if we succeed,
        or INVALID_SOCKET if we fail.
    **************************************************************************/
    SOCKET
    ConnectToSocket(
        u_long  nRemoteAddr,
        u_short nPort);


    SOCKET mSocket;
};

#endif // BLOCKINGCLIENT_H

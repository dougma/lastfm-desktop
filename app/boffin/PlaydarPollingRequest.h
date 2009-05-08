/***************************************************************************
 *   Copyright 2009 Last.fm Ltd.                                           *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#ifndef PLAYDAR_POLLING_REQUEST_H
#define PLAYDAR_POLLING_REQUEST_H

#include <string>
#include "json_spirit/json_spirit.h"


// abstract base class for polling kind of requests
//
class PlaydarPollingRequest
{
public:
    PlaydarPollingRequest();
    virtual ~PlaydarPollingRequest();

    void start();

    const std::string& qid();

private:
    virtual void issueRequest() = 0;
    virtual void issuePoll(unsigned msDelay) = 0;

    // return true if another poll should be made
    virtual bool handleJsonPollResponse(
        int poll, 
        const json_spirit::Object& query, 
        const json_spirit::Array& results) = 0;

    virtual void fail(const char* message) = 0;

protected:
    // derived class calls this with the response of the initial request
    void handleResponse(const char *data, unsigned size);

    // derived class calls this with the response from a poll
    void handlePollResponse(const char *data, unsigned size);

private:
    std::string m_qid;
    int m_pollCount;
};

#endif

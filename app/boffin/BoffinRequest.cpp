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

#include <QNetworkReply>
#include <QTimer>

#include <boost/foreach.hpp>
#include "json_spirit/json_spirit.h"
#include "lib/lastfm/ws/WsAccessManager.h"

#include "BoffinRequest.h"
#include "jsonGetMember.hpp"


BoffinRequest::BoffinRequest(WsAccessManager* wam, PlaydarApi& api)
: m_wam(wam)
, m_api(api)
, m_tagcloudReply(0)
, m_pollReply(0)
{
}

BoffinRequest::~BoffinRequest()
{
    delete m_tagcloudReply;
    delete m_pollReply;
}


void 
BoffinRequest::onReqFinished()
{
    QNetworkReply *reply = (QNetworkReply*) sender();
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray ba( reply->readAll() );
        handleResponse(ba.constData(), ba.size());
    }
    fail("");
}

void 
BoffinRequest::onPollFinished()
{
    QNetworkReply *reply = (QNetworkReply*) sender();
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray ba( reply->readAll() );
        handlePollResponse(ba.constData(), ba.size());
    }
    fail("");
}

// virtual 
void 
BoffinRequest::issueRequest()
{
    m_tagcloudReply = m_wam->get(
        QNetworkRequest(
            m_api.boffinTagcloud()));

    if (m_tagcloudReply) {
        connect(m_tagcloudReply, SIGNAL(finished()), this, SLOT(onReqFinished()));
    } else {
        fail("couldn't issue boffin request");
    }
}

// virtual 
void 
BoffinRequest::issuePoll(unsigned msDelay)
{
    QTimer::singleShot(msDelay, this, SLOT(onTimer()));
}


// virtual
bool
BoffinRequest::handleJsonPollResponse(int poll, 
                                      const json_spirit::Object& , 
                                      const json_spirit::Array& results)
{
    QList<BoffinTagItem> taglist;
    BOOST_FOREACH(const json_spirit::Value& i, results) {
        std::string tagName, hostName;
        int count;
        double weight;
        if (jsonGetMember(i, "name", tagName) &&
            jsonGetMember(i, "source", hostName) &&
            jsonGetMember(i, "count", count) &&
            jsonGetMember(i, "weight", weight))
        {
            taglist << BoffinTagItem(tagName, hostName, count, static_cast<float>(weight));
        }
    }
    emit tags(taglist);
    return poll < 4;
}

void
BoffinRequest::onTimer()
{
    // start the poll now
    delete m_pollReply; // free any previous

    m_pollReply = m_wam->get(
        QNetworkRequest(
            m_api.getResults(
                QString::fromStdString(qid()))));

    if (m_pollReply) {
        connect(m_pollReply, SIGNAL(finished()), this, SLOT(onPollFinished()));
    } else {
        fail("couldn't poll for boffin results");
    }
}

// returns true if another poll should be made
// virtual 


//virtual 
void 
BoffinRequest::fail(const char* message)
{
    message;
    emit error();
}


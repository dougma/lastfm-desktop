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
#include "PlaydarCometRequest.h"
#include <lastfm/NetworkAccessManager>
#include "comet/CometParser.h"


//todo
QString makeGuid()
{
    return "foobar";
}

PlaydarCometRequest::PlaydarCometRequest()
: m_parser(0)
, m_reply(0)
{
}

PlaydarCometRequest::~PlaydarCometRequest()
{
    if (m_parser) m_parser->deleteLater();
    if (m_reply) m_reply->deleteLater();
}

void 
PlaydarCometRequest::start(lastfm::NetworkAccessManager* wam, PlaydarApi& api)
{
    m_id = makeGuid();
    m_reply = wam->get(QNetworkRequest(api.comet(m_id)));
    if (m_reply) {
        m_parser = new CometParser();
        connect(m_parser, SIGNAL(haveObject(QVariantMap)), SIGNAL(receivedObject(QVariantMap)));
        connect(m_reply, SIGNAL(readyRead()), SLOT(onReadyRead()));
        connect(m_reply, SIGNAL(finished()), SLOT(onFinished()));
    } else {
        fail("couldn't issue comet request");
    }
}

void 
PlaydarCometRequest::onReadyRead()
{
    if (!m_parser->push( m_reply->readAll())) {
        fail("json comet parse problem");
    }
}

void
PlaydarCometRequest::fail(const char* message)
{
    Q_UNUSED(message);
    emit error();
}

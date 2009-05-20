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

#include <QUuid>
#include <QNetworkReply>
#include "PlaydarCometRequest.h"
#include <lastfm/NetworkAccessManager>
#include "comet/CometParser.h"


PlaydarCometRequest::PlaydarCometRequest()
:m_parser(0)
,m_sessionId( QUuid::createUuid().toString().mid(1, 36) )
{
}

// returns the sessionId, empty string if request fails
bool
PlaydarCometRequest::issueRequest(lastfm::NetworkAccessManager* wam, PlaydarApi& api)
{
    QNetworkReply* reply = wam->get(QNetworkRequest(api.comet(m_sessionId)));
    if (!reply) {
        return false;
    }

    m_parser = new CometParser(this);
    connect(m_parser, SIGNAL(haveObject(QVariantMap)), SIGNAL(receivedObject(QVariantMap)));
    connect(reply, SIGNAL(readyRead()), SLOT(onFirstReadyRead()));
    connect(reply, SIGNAL(readyRead()), SLOT(onReadyRead()));
    connect(reply, SIGNAL(finished()), SLOT(onFinished()));
    connect(reply, SIGNAL(error( QNetworkReply::NetworkError )), SIGNAL( error()));
    return true;
}

void
PlaydarCometRequest::onReadyRead()
{
    QNetworkReply* reply = (QNetworkReply*) sender();
    if (!m_parser->push(reply->readAll())) {
        qDebug() << "json comet parse problem";
        reply->abort();     // can't recover from this
        onFinished();       // assuming abort() doesn't emit finished
    }
}

void
PlaydarCometRequest::onFirstReadyRead()
{
    sender()->disconnect(this, SLOT(onFirstReadyRead()));
    emit connected(m_sessionId);
}

void
PlaydarCometRequest::onFinished()
{
    sender()->deleteLater();
    emit finished();
}

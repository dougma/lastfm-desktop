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


// returns the sessionId, empty string if request fails
QString 
PlaydarCometRequest::issueRequest(lastfm::NetworkAccessManager* wam, PlaydarApi& api)
{
    QString sessionId( QUuid::createUuid().toString().mid(1, 36) );
    QNetworkReply* reply = wam->get(QNetworkRequest(api.comet(sessionId)));
    if (!reply) {
        return "";
    }

    m_parser = new CometParser(this);
    connect(m_parser, SIGNAL(haveObject(QVariantMap)), SIGNAL(receivedObject(QVariantMap)));
    connect(reply, SIGNAL(readyRead()), SLOT(onReadyRead()));
    connect(reply, SIGNAL(finished()), SLOT(onFinished()));
    return sessionId;
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
PlaydarCometRequest::onFinished()
{
    sender()->deleteLater();
    emit finished();
}
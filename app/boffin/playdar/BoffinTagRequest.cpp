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
#include <lastfm/NetworkAccessManager>
#include "PlaydarApi.h"
#include "BoffinTagRequest.h"
#include "jsonGetMember.h"



// virtual 
void 
BoffinTagRequest::issueRequest(lastfm::NetworkAccessManager* wam, PlaydarApi& api, const QString& rql, const QString& session)
{
    QNetworkReply* reply = wam->get(QNetworkRequest(api.boffinTagcloud(session, qid(), rql)));
    if (reply) {
        connect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
        emit requestMade( qid());
    } else {
        fail("couldn't issue boffin request");
    }
}


void 
BoffinTagRequest::onFinished()
{
    sender()->deleteLater();
    QNetworkReply *reply = (QNetworkReply*) sender();
    if (reply->error() == QNetworkReply::NoError) {
        QString queryId;
        if (getQueryId(reply->readAll(), queryId)) {
            if (queryId == qid()) {
                // all is good
                return; 
            }
            fail("qid mismatch");            // we can't handle this
        }
        fail("bad response");
    }
    fail("");
}


// virtual
void
BoffinTagRequest::receiveResult(const QVariantMap& o)
{
    QString tagName, hostName;
    int count;
    double weight;
    if (jsonGetMember(o, "name", tagName) &&
        jsonGetMember(o, "source", hostName) &&
        jsonGetMember(o, "count", count) &&
        jsonGetMember(o, "weight", weight))
    {
        emit tagItem(BoffinTagItem(tagName, hostName, count, static_cast<float>(weight)));
    }
}

void 
BoffinTagRequest::fail(const char* message)
{
    qDebug() << message;
    emit error();
}

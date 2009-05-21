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
#include "BoffinRqlRequest.h"
#include "jsonGetMember.h"


void
BoffinRqlRequest::onFinished()
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

void
BoffinRqlRequest::issueRequest(lastfm::NetworkAccessManager* wam, PlaydarApi& api, const QString& rql, const QString& session)
{
    QNetworkReply *reply = wam->get(QNetworkRequest(api.boffinRql(session, qid(), rql)));
    if (reply) {
        connect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
        emit requestMade( qid());
    } else {
        fail("couldn't issue boffin rql request");
    }
}

//virtual
void
BoffinRqlRequest::receiveResult(const QVariantMap& o)
{
    int duration = 0;
    jsonGetMember(o, "duration", duration);

    QString artist, album, track, source, mimetype, url;
    if (jsonGetMember(o, "artist", artist) &&
        jsonGetMember(o, "album", album) &&
        jsonGetMember(o, "track", track) &&
        jsonGetMember(o, "source", source) &&
        jsonGetMember(o, "mimetype", mimetype) &&
        jsonGetMember(o, "url", url) )
    {
        emit playableItem(BoffinPlayableItem(artist, album, track, source, mimetype, url, duration));
    }
}

void
BoffinRqlRequest::fail(const char *message)
{
    qDebug() << message;
	emit error();
}

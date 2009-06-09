/*
   Copyright 2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "PlaydarRosterRequest.h"
#include <lastfm/NetworkAccessManager>
#include <QStringList>
#include <QNetworkReply>
#include <QNetworkRequest>
#include "jsonGetMember.h"


PlaydarRosterRequest::PlaydarRosterRequest(lastfm::NetworkAccessManager* wam, PlaydarApi& api)
:m_wam(wam)
,m_api(api)
{
}

void 
PlaydarRosterRequest::start()
{
    QNetworkReply* reply = m_wam->get(QNetworkRequest(m_api.lanRoster()));
    if (reply) {
        connect(reply, SIGNAL(finished()), SLOT(onFinished()));
    } else {
        fail("couldn't issue lan roster request");
    }
}

void 
PlaydarRosterRequest::onFinished()
{
    sender()->deleteLater();
    QNetworkReply *reply = (QNetworkReply*) sender();
    if (reply->error() == QNetworkReply::NoError) {
        using namespace std;

        json_spirit::Value v;
        QByteArray ba( reply->readAll() );
        string sReply( ba.constData(), ba.size() );
        if (json_spirit::read(sReply, v) &&
            v.type() == json_spirit::array_type) 
        {
            QStringList result;

            for (size_t i = 0; i < v.get_array().size(); i++) {
                string name;
                if (jsonGetMember(v.get_array()[i], "name", name)) {
                    result.append(QString::fromStdString(name));
                }
            }

            if (result.size()) {
                emit roster(result);
            }
            return;
        }
        fail("bad json in poll response");
    }
    fail("");
}

void 
PlaydarRosterRequest::fail(const char* )
{
    emit error();
}

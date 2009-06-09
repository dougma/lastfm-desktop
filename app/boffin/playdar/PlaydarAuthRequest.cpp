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

#include <QNetworkReply>
#include "PlaydarAuthRequest.h"
#include <lastfm/NetworkAccessManager>
#include "jsonGetMember.h"

PlaydarAuthRequest::PlaydarAuthRequest(lastfm::NetworkAccessManager* wam, PlaydarApi& api)
:m_wam(wam)
,m_api(api)
{
}

void 
PlaydarAuthRequest::start(QString applicationName)
{
    m_applicationName = applicationName;

    QNetworkReply* auth1Reply = m_wam->get( QNetworkRequest( m_api.auth1(applicationName) ) );
    if (auth1Reply) {
        connect(auth1Reply, SIGNAL(finished()), SLOT(onAuth1Finished()));
    } else {
        fail("couldn't issue auth_1 request");
    }
}

void 
PlaydarAuthRequest::onAuth1Finished()
{
    QNetworkReply *reply = (QNetworkReply*) sender();
    if (reply->error() == QNetworkReply::NoError) {
        using namespace std;

        json_spirit::Value v;
        QByteArray ba( reply->readAll() );
        if (json_spirit::read( string( ba.constData(), ba.size() ), v) ) {
            string formtoken;
            if (jsonGetMember(v, "formtoken", formtoken)) {
                ParamList params;
                QUrl url = m_api.auth2( m_applicationName, QString::fromStdString(formtoken), params );

                // form encode:
			    QByteArray form;
                typedef QPair<QString,QString> Param;
			    foreach (Param p, params) {
                    if (form.size()) {
                        form += "&";
                    }
				    form += QUrl::toPercentEncoding( p.first ) + "="
					      + QUrl::toPercentEncoding( p.second );
			    }

                QNetworkReply* auth2Reply = m_wam->post(QNetworkRequest(url), form);
                if (auth2Reply) {
                    connect(auth2Reply, SIGNAL(finished()), SLOT(onAuth2Finished()));
                    return;
                }
                fail("couldn't issue auth_2 request");
            }
        }
        fail("bad json in auth1 response");
    }
    fail("");

}

void
PlaydarAuthRequest::onAuth2Finished()
{
    QNetworkReply *reply = (QNetworkReply*) sender();
    if (reply->error() == QNetworkReply::NoError) {
        using namespace std;

        json_spirit::Value v;
        QByteArray ba( reply->readAll() );
        if (json_spirit::read( string( ba.constData(), ba.size() ), v) ) {
            string authtoken;
            if (jsonGetMember(v, "authtoken", authtoken)) {
                emit authed(QString::fromStdString(authtoken));
                return;
            }
        }
        fail("bad json in auth2 response");
    }
    fail("");
}

void 
PlaydarAuthRequest::fail(const char*)
{
    emit error();
}

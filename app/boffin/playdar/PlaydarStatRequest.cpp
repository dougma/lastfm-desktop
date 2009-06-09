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
#include "PlaydarStatRequest.h"
#include <lastfm/NetworkAccessManager>
#include "jsonGetMember.h"

PlaydarStatRequest::PlaydarStatRequest(lastfm::NetworkAccessManager* wam, PlaydarApi& api)
:m_wam(wam)
,m_api(api)
,m_statReply(0)
{
}

void 
PlaydarStatRequest::start()
{
    delete m_statReply;
    m_statReply = 0;

    m_statReply = m_wam->get( QNetworkRequest( m_api.stat() ) );
    if (m_statReply) {
        connect(m_statReply, SIGNAL(finished()), SLOT(onReqFinished()));
        connect(m_statReply, SIGNAL(error( QNetworkReply::NetworkError )), SLOT(onError( QNetworkReply::NetworkError )));
    } else {
        fail("couldn't issue stat request");
    }
}

void 
PlaydarStatRequest::onReqFinished()
{
    QNetworkReply *reply = (QNetworkReply*) sender();
    if (reply->error() == QNetworkReply::NoError) {
        using namespace std;

        json_spirit::Value v;
        QByteArray ba( reply->readAll() );
        if (json_spirit::read( string( ba.constData(), ba.size() ), v) ) {
            // note: hostname is only present when authenticated is true
            string name, version, hostname;
            bool authenticated;
            if (jsonGetMember(v, "name", name) &&
                jsonGetMember(v, "version", version) &&
                jsonGetMember(v, "authenticated", authenticated) &&
                (!authenticated || jsonGetMember(v, "hostname", hostname)) )
            {
                emit stat(QString::fromStdString(name), 
                    QString::fromStdString(version), 
                    QString::fromStdString(hostname), 
                    authenticated);
                return;
            }
        }
        fail("bad json in poll response");
    }
    fail("");
}

void 
PlaydarStatRequest::onError( QNetworkReply::NetworkError code )
{
    QObject* s = sender();
    qDebug() << "StatRequest error: " << code << endl;
}

void 
PlaydarStatRequest::fail(const char* message)
{
    emit error();
}
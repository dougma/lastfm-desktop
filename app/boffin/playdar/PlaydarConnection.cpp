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

#include <QTimer>
#include "PlaydarConnection.h"
#include "PlaydarStatRequest.h"
#include "PlaydarAuthRequest.h"
#include "PlaydarRosterRequest.h"
#include "PlaydarCometRequest.h"
#include "TrackResolveRequest.h"
#include "BoffinRqlRequest.h"
#include "BoffinTagRequest.h"
#include <lastfm/NetworkAccessManager>


PlaydarConnection::PlaydarConnection(lastfm::NetworkAccessManager* wam, PlaydarApi& api)
: m_comet(0)
, m_wam(wam)
, m_api(api)
, m_state(Querying)
{
    updateText();
}

void
PlaydarConnection::start()
{
    PlaydarStatRequest* stat = new PlaydarStatRequest(m_wam, m_api);
    connect(stat, SIGNAL(stat(QString, QString, QString, bool)), SLOT(onStat(QString, QString, QString, bool)));
    connect(stat, SIGNAL(error()), SLOT(onError()));
    stat->start();
}

void
PlaydarConnection::onStat(QString name, QString version, QString hostname, bool bAuthenticated)
{
    m_name = name;
    m_version = version;
    m_hostname = hostname;
    m_state = bAuthenticated ? Connecting : Authorising;
    if (!bAuthenticated) {
        PlaydarAuthRequest* auth = new PlaydarAuthRequest(m_wam, m_api);
        connect(auth, SIGNAL(authed(QString)), SLOT(onAuth(QString)));
        connect(auth, SIGNAL(authed(QString)), SIGNAL(authed(QString)));
        connect(auth, SIGNAL(error()), SLOT(onError()));
        auth->start("Boffin");
    } else {
        makeCometRequest();
    }

    updateText();
}

void
PlaydarConnection::onError()
{
    sender()->deleteLater();
    switch (m_state) {
        case Querying :
            m_state = NotPresent;
            break;
        case Authorising :
            m_state = NotAuthorised;
            break;
        case Connecting:
            m_state = Connecting;
            break;
        case Connected :
            m_state = Querying;
            start();
            break;
        default:
            break;
    }
    updateText();
}

void
PlaydarConnection::onAuth(QString authToken)
{
    sender()->deleteLater();
    m_api.setAuthToken(authToken);
    m_state = Connecting;
    updateText();

    makeCometRequest();
}

void
PlaydarConnection::onLanRoster(const QStringList& roster)
{
    sender()->deleteLater();
    m_hostsModel.setStringList(roster);
    QTimer::singleShot(60 * 1000, this, SLOT(makeRosterRequest()));
}

void
PlaydarConnection::makeRosterRequest()
{
    PlaydarRosterRequest* req = new PlaydarRosterRequest(m_wam, m_api);
    connect(req, SIGNAL(roster(QStringList)), SLOT(onLanRoster(QStringList)));
    connect(req, SIGNAL(error()), SLOT(onError()));
    req->start();
}

void
PlaydarConnection::makeCometRequest()
{
    m_comet = new PlaydarCometRequest();
    if (m_comet->issueRequest(m_wam, m_api)) {
        connect(m_comet, SIGNAL(connected(QString)), SLOT(onCometConnected(QString)));
        connect(m_comet, SIGNAL(error()), SLOT(onError()));
        connect(m_comet, SIGNAL(receivedObject(QVariantMap)), SLOT(receivedCometObject(QVariantMap)));
    }
}

void
PlaydarConnection::onCometConnected(const QString& sessionId)
{
    m_cometSession = sessionId;
    m_state = Connected;
    updateText();
    emit connected();
}

void
PlaydarConnection::updateText()
{
    QString s;
    switch (m_state) {
        case Querying: s = "Looking for Playdar"; break;
        case NotPresent: s = "Playdar not available"; break;
        case Authorising: s = "Authorising with Playdar"; break;
        case NotAuthorised: s = "Couldn't authorise with Playdar"; break;
        case Connecting: s = "Connecting to Playdar"; break;
        case Connected: s = "Connected to Playdar"; break;
        default:
            s = "PlaydarConnection::updateText is broken!";
    }
    emit changed(s);
}

QStringListModel*
PlaydarConnection::hostsModel()
{
    return &m_hostsModel;
}

TrackResolveRequest* 
PlaydarConnection::trackResolve(const QString& artist, const QString& album, const QString& track)
{
    if (!m_cometSession.length()) {
        return 0;
    }
    TrackResolveRequest* r = new TrackResolveRequest();
    connect(r, SIGNAL(requestMade(QString)), SLOT(onRequestMade(QString)));
    connect(r, SIGNAL(destroyed(QObject*)), SLOT(onRequestDestroyed(QObject*)));
    r->issueRequest(m_wam, m_api, artist, album, track, m_cometSession);
    return r;
}

BoffinRqlRequest*
PlaydarConnection::boffinRql(const QString& rql)
{
    if (!m_cometSession.length()) {
        return 0;
    }
    BoffinRqlRequest* r = new BoffinRqlRequest();
    connect(r, SIGNAL(requestMade(QString)), SLOT(onRequestMade(QString)));
    connect(r, SIGNAL(destroyed(QObject*)), SLOT(onRequestDestroyed(QObject*)));
    r->issueRequest(m_wam, m_api, rql, m_cometSession);
    return r;
}

BoffinTagRequest*
PlaydarConnection::boffinTagcloud(const QString& rql)
{
    if (!m_cometSession.length()) {
        return 0;
    }
    BoffinTagRequest* r = new BoffinTagRequest();
    connect(r, SIGNAL(requestMade(QString)), SLOT(onRequestMade(QString)));
    connect(r, SIGNAL(destroyed(QObject*)), SLOT(onRequestDestroyed(QObject*)));
    r->issueRequest(m_wam, m_api, rql, m_cometSession);
    return r;
}

void
PlaydarConnection::onRequestMade(const QString& qid)
{
    m_cometReqMap[qid] = (CometRequest*) sender();
}

void
PlaydarConnection::onRequestDestroyed(QObject* o)
{
    m_cometReqMap.remove(((CometRequest*)o)->qid());
}

void
PlaydarConnection::receivedCometObject(const QVariantMap& obj)
{
    QVariantMap::const_iterator qit = obj.find("query");
    if (qit != obj.end() && qit->type() == QVariant::String) {
        QVariantMap::const_iterator rit = obj.find("result");
        if (rit != obj.end() && rit->type() == QVariant::Map) {
            // obj is the right shape, find it in
            // the request map and emit the callback
            QMap<QString, CometRequest*>::const_iterator reqIt = m_cometReqMap.find(qit->toString());
            if (reqIt != m_cometReqMap.end()) {
                reqIt.value()->receiveResult(rit->toMap());
            } else {
                // unknown query id.
                qDebug() << "warning: result for unknown query " << qit->toString() << " was discarded";
            }
        }
    }
}


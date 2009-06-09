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

#ifndef PLAYDAR_CONNECTION_H
#define PLAYDAR_CONNECTION_H

#include "PlaydarApi.h"
#include <lastfm/global.h>
#include <QStringListModel>

class PlaydarCometRequest;
class CometRequest;
class TrackResolveRequest;
class BoffinTagRequest;
class BoffinRqlRequest;

class PlaydarConnection : public QObject
{
    Q_OBJECT

public:
    PlaydarConnection(lastfm::NetworkAccessManager* wam, PlaydarApi& api);

    void start();
    QStringListModel* hostsModel();

    TrackResolveRequest* trackResolve(const QString& artist, const QString& album, const QString& track);
    BoffinTagRequest* boffinTagcloud(const QString& rql);
    BoffinRqlRequest* boffinRql(const QString& rql);

signals:
    void changed(QString newStatusMessage);
    void authed(QString authtoken);
    void connected();

private slots:
    void onStat(QString name, QString version, QString hostname, bool bAuthenticated);
    void onAuth(QString authToken);
    void onLanRoster(const QStringList& roster);
    void onError();
    void makeRosterRequest();

    void onCometConnected(const QString& sessionId);
    void receivedCometObject(const QVariantMap&);
    void onRequestMade(const QString& qid);
    void onRequestDestroyed(QObject* o);

private:
    void updateText();
    void makeCometRequest();
    QString cometSession();

    enum State
    {
        Querying, NotPresent, Authorising, NotAuthorised, Connecting, Connected
    };

    QString m_name;
    QString m_version;
    QString m_hostname;

    QStringListModel m_hostsModel;

    PlaydarCometRequest* m_comet;
    QString m_cometSession;
    QMap<QString, CometRequest*> m_cometReqMap;

    lastfm::NetworkAccessManager* m_wam;
    PlaydarApi& m_api;
    State m_state;
};

#endif


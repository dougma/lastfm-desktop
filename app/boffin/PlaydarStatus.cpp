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

#include <QTimer>
#include "PlaydarStatus.h"
#include "PlaydarStatRequest.h"
#include "PlaydarAuthRequest.h"
#include "PlaydarRosterRequest.h"
#include <lastfm/NetworkAccessManager>


PlaydarStatus::PlaydarStatus(lastfm::NetworkAccessManager* wam, PlaydarApi& api)
: m_wam(wam)
, m_api(api)
, m_state(Connecting)
{
    updateText();
}

void
PlaydarStatus::start()
{
    PlaydarStatRequest* stat = new PlaydarStatRequest(m_wam, m_api);
    connect(stat, SIGNAL(stat(QString, QString, QString, bool)), SLOT(onStat(QString, QString, QString, bool)));
    connect(stat, SIGNAL(error()), SLOT(onError()));
    stat->start();
}

void 
PlaydarStatus::onStat(QString name, QString version, QString hostname, bool bAuthenticated)
{
    m_name = name;
    m_version = version;
    m_hostname = hostname;
    m_state = bAuthenticated ? Authorised : Authorising;
    if (!bAuthenticated) {
        PlaydarAuthRequest* auth = new PlaydarAuthRequest(m_wam, m_api);
        connect(auth, SIGNAL(authed(QString)), SLOT(onAuth(QString)));
        connect(auth, SIGNAL(error()), SLOT(onError()));
        auth->start("Boffin");
    }
    updateText();
}

void
PlaydarStatus::onError()
{
    sender()->deleteLater();
    switch (m_state) {
        case Connecting : 
            m_state = NotPresent; 
            break;
        case Authorising : 
            m_state = NotAuthorised; 
            break;
        case Authorised : 
            m_state = Connecting; 
            start();
            break;
        default:
            break;
    }
    updateText();
}

void
PlaydarStatus::onAuth(QString authToken)
{
    sender()->deleteLater();
    m_api.setAuthToken(authToken);
    m_state = Authorised;
    updateText();
    emit connected();
    makeRosterRequest();
}

void
PlaydarStatus::onLanRoster(const QStringList& roster)
{
    sender()->deleteLater();
    m_hostsModel.setStringList(roster);
    QTimer::singleShot(60 * 1000, this, SLOT(makeRosterRequest()));
}

void
PlaydarStatus::makeRosterRequest()
{
    PlaydarRosterRequest* req = new PlaydarRosterRequest(m_wam, m_api);
    connect(req, SIGNAL(roster(QStringList)), SLOT(onLanRoster(QStringList)));
    connect(req, SIGNAL(error()), SLOT(onError()));
    req->start();
}

void
PlaydarStatus::updateText()
{
    QString s;
    switch (m_state) {
        case Connecting: s = "Connecting to Playdar"; break;
        case NotPresent: s = "Playdar not available"; break;
        case Authorising: s = "Authorising with Playdar"; break;
        case Authorised: s = "Connected to Playdar"; break;
        case NotAuthorised: s = "Couldn't authorise with Playdar"; break;
        default: 
            s = "PlaydarStatus::updateText is broken!";
    }
    emit changed(s);
}

QStringListModel*
PlaydarStatus::hostsModel()
{
    return &m_hostsModel;
}

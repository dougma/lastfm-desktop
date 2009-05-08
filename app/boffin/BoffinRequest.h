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

#ifndef BOFFIN_REQUEST_H
#define BOFFIN_REQUEST_H

#include <lastfm/global.h>
#include "PlaydarApi.h"
#include "PlaydarPollingRequest.h"


struct BoffinTagItem
{
    BoffinTagItem(const std::string &name, const std::string &host, int count, float weight)
        : m_name(QString::fromStdString(name))
        , m_host(QString::fromStdString(host))
        , m_count(count)
        , m_weight(weight)
    {
    }

    QString m_name;
    QString m_host;
    int m_count;
    float m_weight;
};

class BoffinRequest 
    : public QObject
    , public PlaydarPollingRequest
{
    Q_OBJECT

public:
    BoffinRequest(lastfm::NetworkAccessManager* wam, PlaydarApi& api);
    ~BoffinRequest();

signals:
    void error();
    void tags(QList<BoffinTagItem> tags);

private slots:
    void onReqFinished();
    void onPollFinished();
    void onTimer();

private:
    virtual void issueRequest();
    virtual void issuePoll(unsigned msDelay);
    virtual bool handleJsonPollResponse(int poll, const json_spirit::Object& query, const json_spirit::Array& results);
    virtual void fail(const char* message);

    PlaydarApi m_api;
    lastfm::NetworkAccessManager *m_wam;
    QNetworkReply *m_tagcloudReply;
    QNetworkReply *m_pollReply;
};

#endif

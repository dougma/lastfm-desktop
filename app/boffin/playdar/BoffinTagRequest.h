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

#ifndef BOFFIN_REQUEST_H
#define BOFFIN_REQUEST_H

#include <lastfm/global.h>
#include "PlaydarApi.h"
#include "CometRequest.h"


struct BoffinTagItem
{
	BoffinTagItem()
	{}

	BoffinTagItem( const QString& name ): m_name( name ) {};

    BoffinTagItem(const QString& name, const QString& host, int count, float weight, int seconds)
        : m_name(name)
        , m_host(host)
        , m_count(count)
        , m_weight(weight)
        , m_seconds(seconds)
    {
    }

    bool operator==( const BoffinTagItem& that)
    {
    	return m_name == that.m_name;
    }

    bool operator <( const BoffinTagItem& that ) const
    {
    	return m_count < that.m_count;
    }

    QString m_name;
    QString m_host;
    int m_count;
    float m_weight;
    float m_logWeight;
    float m_logCount;
    int m_seconds;          // the total duration of tracks with this tag
};

class BoffinTagRequest : public CometRequest
{
    Q_OBJECT

public:
    void issueRequest(lastfm::NetworkAccessManager* wam, PlaydarApi& api, const QString& rql, const QString& session);
    virtual void receiveResult(const QVariantMap& o);

signals:
    void error();
    void tagItem(BoffinTagItem item);
    void requestMade( const QString );
private slots:
    void onFinished();

private:
    void fail(const char* message);
};

#endif

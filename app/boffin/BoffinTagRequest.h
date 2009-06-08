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
#include "CometRequest.h"


struct BoffinTagItem
{
	BoffinTagItem()//: m_valid( false )
	{}

	BoffinTagItem( const QString& name ): m_name( name )/*, m_valid( false )*/ {};

    BoffinTagItem(const QString& name, const QString& host, int count, float weight)
        : m_name(name)
        , m_host(host)
        , m_count(count)
        , m_weight(weight)
//        , m_valid( true )
    {
    }

    bool operator==( const BoffinTagItem& that)
    {
    	return m_name == that.m_name;
    }

    bool operator <( const BoffinTagItem& that ) const
    {
    	return m_weight < that.m_weight;
    }

    QString m_name;
    QString m_host;
    int m_count;
    float m_weight;
    float m_logWeight;
    //bool m_valid;
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

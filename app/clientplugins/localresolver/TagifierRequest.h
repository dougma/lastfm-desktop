/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
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

#ifndef TAGIFIER_REQUEST_H
#define TAGIFIER_REQUEST_H

#include <QUrl>
#include <QByteArray>
#include <QVariantList>
#include "lib/lastfm/ws/WsAccessManager.h"


class TagifierRequest : public QObject
{
    Q_OBJECT;

    QUrl m_url;
    QByteArray m_body;
    WsAccessManager m_wam;
    class QNetworkReply* m_reply;
    class LocalCollection& m_collection;
    int m_requestIdCount, m_responseIdCount, m_responseTagCount;
    QVariantList m_requestedFileIds;

    void handleResponse();

private slots:
    void onFinished();

public:
//  "http://compute1:8080/trackresolve"

    TagifierRequest(LocalCollection& collection, QString url);
    bool makeRequest();

signals:
    void finished(int requestIdCount, int responseIdCount, int responseTagCount);
};

#endif
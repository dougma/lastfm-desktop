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

#ifndef PLAYDAR_STAT_REQUEST_H
#define PLAYDAR_STAT_REQUEST_H

#include <lastfm/global.h>
#include <QPointer>
#include <QObject>
#include "PlaydarApi.h"

class PlaydarStatRequest
    : public QObject
{
    Q_OBJECT

public:
    PlaydarStatRequest(lastfm::NetworkAccessManager*, PlaydarApi&);
    void start();

signals:
    void error();
    void stat(QString name, QString version, QString hostname, bool bAuthenticated);

private slots:
    void onReqFinished();

private:
    void fail(const char* message);

    lastfm::NetworkAccessManager* m_wam;
    PlaydarApi m_api;
    QPointer<QNetworkReply> m_statReply;
};

#endif

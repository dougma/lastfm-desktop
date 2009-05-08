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

#ifndef PLAYDAR_AUTH_REQUEST_H
#define PLAYDAR_AUTH_REQUEST_H

#include "PlaydarApi.h"
#include <lastfm/global.h>
#include <QObject>


class PlaydarAuthRequest
    : public QObject
{
    Q_OBJECT

public:
    PlaydarAuthRequest(lastfm::NetworkAccessManager* wam, PlaydarApi& api);
    void start(QString applicationName);

signals:
    void error();
    void authed(QString token);

private slots:
    void onAuth1Finished();
    void onAuth2Finished();

private:
    void fail(const char* message);

    lastfm::NetworkAccessManager* m_wam;
    PlaydarApi m_api;
    QString m_applicationName;
};

#endif

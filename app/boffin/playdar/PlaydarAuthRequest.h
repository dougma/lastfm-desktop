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

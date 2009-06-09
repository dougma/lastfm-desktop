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

#ifndef BOFFIN_RQL_REQUEST_H
#define BOFFIN_RQL_REQUEST_H

#include <lastfm/global.h>
#include "PlaydarApi.h"
#include "CometRequest.h"
#include "BoffinPlayableItem.h"

class BoffinRqlRequest : public CometRequest
{
    Q_OBJECT

public:
    void issueRequest(lastfm::NetworkAccessManager* wam, PlaydarApi& api, const QString& rql, const QString& session);
    virtual void receiveResult(const QVariantMap& o);

signals:
    void error();
    void playableItem(BoffinPlayableItem item);
    void requestMade( const QString );

private slots:
    void onFinished();

private:
    void fail(const char* message);
};

#endif


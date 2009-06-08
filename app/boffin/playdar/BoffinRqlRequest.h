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


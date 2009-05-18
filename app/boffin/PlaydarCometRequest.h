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

#ifndef PLAYDAR_COMET_REQUEST_H
#define PLAYDAR_COMET_REQUEST_H

#include "PlaydarApi.h"
#include <lastfm/global.h>
#include <QVariant>

class CometParser;

// makes a request to playdar for comet results, 
// emits signals as result objects arrive
//
class PlaydarCometRequest : public QObject
{
    Q_OBJECT

public:
    // returns the sessionId, empty string if request fails
    QString issueRequest(lastfm::NetworkAccessManager* wam, PlaydarApi& api);

signals:
    void receivedObject(QVariantMap);
    void finished();

private slots:
    void onReadyRead();
    void onFinished();

private:
    CometParser *m_parser;
};

#endif

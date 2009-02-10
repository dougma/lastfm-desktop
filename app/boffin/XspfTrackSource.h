/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *    This program is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#ifndef XSPF_TRACK_SOURCE
#define XSPF_TRACK_SOURCE
 
#include <QList>
#include "lib/lastfm/ws/WsAccessManager.h"
#include "lib/lastfm/types/Xspf.h"
#include "lib/lastfm/radio/AbstractTrackSource.h"


// Represent a xspf url as an AbstractTrackSource
//
class XspfTrackSource : public AbstractTrackSource
{
    Q_OBJECT

    QUrl m_url;
    bool m_requested;
    class QNetworkReply *m_reply;
    QList<Track> m_queue;

    void handleXspf(const QDomElement& playlistElement);

private slots:
    void onFinished();

public:
    XspfTrackSource(QUrl);
    Track takeNextTrack();
};

#endif

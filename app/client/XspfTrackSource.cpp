/*
   Copyright 2005-2009 Last.fm Ltd. 
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
#include "XspfTrackSource.h"
#include <QNetworkRequest>
#include <lastfm/WsReply>


 XspfTrackSource::XspfTrackSource(QUrl url)
: m_url(url)
, m_requested(false)
{
}

Track
XspfTrackSource::takeNextTrack()
{
    if (!m_requested) {
        m_requested = true;
        m_reply = (new WsAccessManager(this))->get(QNetworkRequest(m_url));
        connect(m_reply, SIGNAL(finished()), SLOT(onFinished()));
        return Track();
    }
    if (m_queue.isEmpty()) {
        return Track();
    }
    return m_queue.takeFirst();
}

void
XspfTrackSource::onFinished()
{
    QDomDocument xmldoc;
    xmldoc.setContent(m_reply);
    QDomElement docElement(xmldoc.documentElement());
    if (docElement.tagName() == "playlist") {
        handleXspf(docElement);
    } else if (docElement.tagName() == "lfm") {
        handleXspf(docElement.firstChildElement("playlist"));
    } else {
        // todo
    }
    delete m_reply;
    m_reply = 0;
}

void 
XspfTrackSource::handleXspf(const QDomElement& playlistElement)
{
    Xspf xspf(playlistElement, Track::Player);
    emit title(xspf.title());

    m_queue = xspf.tracks();
    if (m_queue.size()) {
        emit trackAvailable();
    }
}

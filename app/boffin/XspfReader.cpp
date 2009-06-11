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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "XspfReader.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <lastfm/NetworkAccessManager>


 XspfReader::XspfReader(QUrl url)
: m_url(url)
{
    m_reply = (new lastfm::NetworkAccessManager(this))->get(QNetworkRequest(m_url));
    connect(m_reply, SIGNAL(finished()), SLOT(onFinished()));
}

void
XspfReader::onFinished()
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
    m_reply->deleteLater();
    m_reply = 0;
}

void 
XspfReader::handleXspf(const QDomElement& playlistElement)
{
    Xspf xspf(playlistElement);
    emit XspfReader::xspf(xspf);
}

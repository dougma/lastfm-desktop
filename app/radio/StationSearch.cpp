/*
   Copyright 2005-2009 Last.fm Ltd. 

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

#include "StationSearch.h"
#include <lastfm/ws.h>
#include <lastfm/XmlQuery>

using namespace lastfm;

void
StationSearch::startSearch(const QString& name)
{
    QMap<QString, QString> params;
    params["method"] = "radio.search";
    params["name"] = name;
    connect(ws::get(params), SIGNAL(finished()), SLOT(onFinished()));
}

void
StationSearch::onFinished()
{
    try {
        sender()->deleteLater();
        lastfm::XmlQuery x(ws::parse(qobject_cast<QNetworkReply*>(sender())));
        lastfm::XmlQuery station = x["stations"]["station"];
        RadioStation rs(station["url"].text());
        rs.setTitle(station["name"].text());
        emit searchResult(rs);
    } catch (...) {
        qDebug() << "exception";
    }
}
    
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

#include <QNetworkReply>
#include <QGridLayout>
#include <QLabel>
#include <lastfm/AuthenticatedUser>
#include <lastfm/XmlQuery>
#include "RecentStationsWidget.h"
#include "PlayableItemWidget.h"

RecentStationsWidget::RecentStationsWidget()
{
    // todo: start a spinner
    update();
}

void
RecentStationsWidget::update()
{
    AuthenticatedUser you;
    connect(you.getRecentStations(), SIGNAL(finished()), SLOT(gotRecentStations()));

    m_layout = new QGridLayout(this);
    m_layout->addWidget(new QLabel(tr("Your Recent Stations")), 0, 0, 1, 2, Qt::AlignCenter);
}

void
RecentStationsWidget::gotRecentStations()
{
    // todo: stop any spinner
    sender()->deleteLater();
    QList<RadioStation> recent = RadioStation::list((QNetworkReply*)sender());

    int p = 0;
    foreach(const RadioStation& rs, recent) {
        PlayableItemWidget* item = new PlayableItemWidget(rs.title(), rs);
        m_layout->addWidget(item, 1 + p / 2, p % 2);
        connect(item, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));
        p++;
    }
    
}

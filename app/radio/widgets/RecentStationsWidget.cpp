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
    // todo: implement AuthenticatedUser::getRecentStations()
    //connect(AuthenticatedUser::getRecentStations(), SIGNAL(finished()), SLOT(gotRecentStations()));

    // dummy code begins {
    QGridLayout* layout = new QGridLayout();
    layout->addWidget(new QLabel(tr("Your Recent Stations")), 0, 0, 1, 2, Qt::AlignCenter);

    PlayableItemWidget* item;
    item = new PlayableItemWidget(tr("Radiohead Radio"), RadioStation::similar(Artist("Radiohead")));
    layout->addWidget(item, 1, 0);
    connect(item, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));

    item = new PlayableItemWidget(tr("mxcl's Loved Tracks"), RadioStation::lovedTracks(User("mxcl")));
    layout->addWidget(item, 1, 1);
    connect(item, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));

    item = new PlayableItemWidget(tr("Jazz Tag Radio"), RadioStation::globalTag(Tag("Jazz")));
    layout->addWidget(item, 2, 0);
    connect(item, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));

    item = new PlayableItemWidget(tr("megadeth radio"), RadioStation::similar(Artist("megadeth")));
    layout->addWidget(item, 2, 1);
    connect(item, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));
    
    layout->addWidget(new QLabel(tr("see more")), 3, 0, 1, 2, Qt::AlignRight);

    setLayout(layout);
    // } ends
}

void
RecentStationsWidget::gotRecentStations()
{
    // todo: stop any spinner
    sender()->deleteLater();
    QNetworkReply* r = (QNetworkReply*)sender();
    XmlQuery lfm = r->readAll();
    // todo: handle response
}

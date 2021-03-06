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
#include <QPushButton>
#include <QVBoxLayout>
#include "MainStarterWidget.h"
#include "QuickStartWidget.h"
#include "RecentStationsWidget.h"
#include "YourStationsWidget.h"
#include "MoreStationsWidget.h"

MainStarterWidget::MainStarterWidget()
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QWidget* w;
    w = new QuickStartWidget();
    connect(w, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));
    layout->addWidget(w);

    w = m_recentStationsWidget = new RecentStationsWidget();
    connect(w, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));
    connect(w, SIGNAL(showMoreRecentStations()), SIGNAL(showMoreRecentStations()));
    layout->addWidget(w);

    w = new YourStationsWidget();
    connect(w, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));
    layout->addWidget(w);

    w = new MoreStationsWidget();
    connect(w, SIGNAL(combo()), SIGNAL(combo()));
    connect(w, SIGNAL(tags()), SIGNAL(yourTags()));
    connect(w, SIGNAL(friends()), SIGNAL(yourFriends()));
    connect(w, SIGNAL(playlists()), SIGNAL(yourPlaylists()));
    layout->addWidget(w);
}

void
MainStarterWidget::setRecentStationsModel(QAbstractItemModel* m)
{
    m_recentStationsWidget->setModel(m);
}
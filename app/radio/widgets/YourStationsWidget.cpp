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

//#include <QComboBox>
//#include <QLineEdit>
//#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <lastfm/RadioStation>
#include "YourStationsWidget.h"
#include "PlayableItemWidget.h"

YourStationsWidget::YourStationsWidget()
{
    QGridLayout* layout = new QGridLayout();

    PlayableItemWidget* item;
    layout->addWidget(new QLabel(tr("Your Stations")), 0, 0, 1, 2, Qt::AlignCenter);

    item = new PlayableItemWidget(tr("Your Library"), RadioStation::library(lastfm::ws::Username));
    layout->addWidget(item, 1, 0);
    connect(item, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));

    item = new PlayableItemWidget(tr("Your Loved Tracks"), RadioStation::lovedTracks(lastfm::ws::Username));
    layout->addWidget(item, 1, 1);
    connect(item, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));

    item = new PlayableItemWidget(tr("Your Recommended Radio"), RadioStation::recommendations(lastfm::ws::Username));
    layout->addWidget(item, 2, 0);
    connect(item, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));

    item = new PlayableItemWidget(tr("Your Neighbourhood"), RadioStation::neighbourhood(lastfm::ws::Username));
    layout->addWidget(item, 2, 1);
    connect(item, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));
    
    setLayout(layout);
}

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
#include <QVBoxLayout>
#include "MainStarterWidget.h"
#include "QuickStartWidget.h"
#include "RecentStationsWidget.h"
#include "YourStationsWidget.h"
#include "MoreStationsWidget.h"

MainStarterWidget::MainStarterWidget()
{
    QWidget* w;
    QVBoxLayout* layout = new QVBoxLayout();

    w = new QuickStartWidget();
    connect(w, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));
    layout->addWidget(w);

    w = new RecentStationsWidget();
    connect(w, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));
    layout->addWidget(w);

    w = new YourStationsWidget();
    connect(w, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));
    layout->addWidget(w);

    w = new MoreStationsWidget();
    connect(w, SIGNAL(combo()), SIGNAL(combo()));
    connect(w, SIGNAL(yourTags()), SIGNAL(yourTags()));
    layout->addWidget(w);

    setLayout(layout);
}

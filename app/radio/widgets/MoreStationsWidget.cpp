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

#include "MoreStationsWidget.h"

#include <QGridLayout>
#include <QLabel>

MoreStationsWidget::MoreStationsWidget()
{
    QGridLayout* layout = new QGridLayout();

    layout->addWidget(new QLabel(tr("More")), 0, 0, 1, 2, Qt::AlignCenter);

    QLabel* item;
    item = new QLabel(tr("Combo Station"));
    layout->addWidget(item, 1, 0);
//    connect(item, SIGNAL(), SLOT(onCombo()));

    item = new QLabel(tr("Your Tags"));
    layout->addWidget(item, 1, 1);
//    connect(item, SIGNAL(), SIGNAL(onYourTags()));

    setLayout(layout);
}


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

#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include "QuickStartWidget.h"
#include "../StationSearch.h"

QuickStartWidget::QuickStartWidget()
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    QPushButton* button = new QPushButton(tr("Play"));
    layout->addWidget(m_edit = new QLineEdit());
    layout->addWidget(button);

    m_edit->setAttribute( Qt::WA_MacShowFocusRect, false );
    
    connect(m_edit, SIGNAL(returnPressed()), SLOT(search()));
    connect(button, SIGNAL(clicked()), SLOT(search()));
}

void
QuickStartWidget::search()
{
    if (m_edit->text().length()) {
        StationSearch* s = new StationSearch();
        connect(s, SIGNAL(searchResult(RadioStation)), SIGNAL(startRadio(RadioStation)));
        s->startSearch(m_edit->text());
    }
}

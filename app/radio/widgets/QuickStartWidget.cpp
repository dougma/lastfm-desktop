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

QuickStartWidget::QuickStartWidget()
{
    m_combo = new QComboBox();
    m_combo->addItem(tr("Artist"));
    m_combo->addItem(tr("Tag"));
    m_edit = new QLineEdit();
    QPushButton* button = new QPushButton(tr("Play"));
    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(m_combo);
    layout->addWidget(m_edit);
    layout->addWidget(button);
    setLayout(layout);

    connect(m_edit, SIGNAL(returnPressed()), SLOT(play()));
    connect(button, SIGNAL(clicked()), SLOT(play()));
}

void
QuickStartWidget::play()
{
    switch (m_combo->currentIndex()) {
        case 0: // artist
            emit startRadio(lastfm::RadioStation::similar(lastfm::Artist(m_edit->text())));
            break;
        case 1: // tag
            emit startRadio(lastfm::RadioStation::globalTag(lastfm::Tag(m_edit->text())));
            break;
        default:
            qDebug() << "?";
    }
    
}


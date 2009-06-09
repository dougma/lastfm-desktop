/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

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
#include <QHBoxLayout>
#include <QPushButton>
#include "HistoryWidget.h"

HistoryWidget::HistoryWidget(QWidget* parent)
    : QWidget(parent)
{
    m_layout = new QHBoxLayout(this);
    m_layout->setAlignment( Qt::AlignLeft );
}

HistoryWidget::~HistoryWidget()
{}

bool 
HistoryWidget::pop()
{
    if (m_labels.count()) {
        QPushButton* label = m_labels.pop();
        m_layout->removeWidget(label);
        delete label;
        return true;
    }
    return false;
}


void 
HistoryWidget::newItem(const QString& text)
{
    QPushButton* label = new QPushButton(text, this);
    m_layout->addWidget(label, 0, Qt::AlignLeft);
    m_labels.push(label);
    connect(label, SIGNAL(clicked()), SLOT(onItemClicked()));
}

void 
HistoryWidget::onItemClicked()
{
    QPushButton* label = static_cast<QPushButton*>(sender());
    for (int i = 0; i < m_labels.count(); i++) {
        if (m_labels[i] == label) {
            emit clicked(i, label->text());
            return;
        }
    }
}

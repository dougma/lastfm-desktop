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

#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QListWidgetItem>
#include "SourceSelectorWidget.h"

SourceSelectorWidget::SourceSelectorWidget(QLineEdit* edit, QWidget* parent)
    :QWidget(parent)
{
    QGridLayout* grid = new QGridLayout(this);

    m_edit = edit;
    m_edit->setParent(this);
    grid->addWidget(m_edit, 0, 0);

    m_button = new QPushButton(tr("Add"));
    grid->addWidget(m_button, 0, 1);
    grid->setColumnStretch(0, 3);

    m_list = new QListWidget();
    m_list->setIconSize(QSize(0,0));
    grid->addWidget(m_list, 1, 0, 1, 2);

    connect(m_edit, SIGNAL(returnPressed()), SLOT(emitAdd())); 
    connect(m_button, SIGNAL(clicked()), SLOT(emitAdd()));
    connect(m_list, SIGNAL(itemActivated(QListWidgetItem *)), SIGNAL(itemActivated(QListWidgetItem *)));
}

QListWidget* 
SourceSelectorWidget::list()
{
    return m_list;
}

void
SourceSelectorWidget::emitAdd()
{
    emit add(m_edit->text());
}

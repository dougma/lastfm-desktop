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
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollBar>
#include <lastfm/AuthenticatedUser>
#include <lastfm/XmlQuery>
#include "RecentStationsWidget.h"
#include "ColumnListWidget.h"
#include "../RadioStationListModel.h"

#define GRID_HEIGHT_PX 30
#define ROW_COUNT 3


RecentStationsWidget::RecentStationsWidget()
{
    m_list = new ColumnListWidget(350, GRID_HEIGHT_PX);
    m_list->setSelectionMode(QAbstractItemView::NoSelection);
    m_list->setWrapping(true);
    m_list->setResizeMode(QListView::Adjust);
    m_list->setUniformItemSizes(false);
    m_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_list->setMaximumHeight(GRID_HEIGHT_PX * ROW_COUNT + 1);
    connect(m_list, SIGNAL(clicked(QModelIndex)), SLOT(onActivate(QModelIndex)));
    connect(m_list, SIGNAL(doubleClicked(QModelIndex)), SLOT(onActivate(QModelIndex)));
    connect(m_list, SIGNAL(columnsChanged()), SLOT(recalcMoreButton()));

    m_moreButton = new QPushButton(tr("more"));
    m_moreButton->setObjectName("more");
    m_moreButton->setVisible(false);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(tr("Your Recent Stations")), 0, Qt::AlignCenter);
    layout->addWidget(m_list);
    layout->addWidget(m_moreButton, 0, Qt::AlignRight);

    connect(m_moreButton, SIGNAL(clicked()), SIGNAL(showMoreRecentStations()));
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

void
RecentStationsWidget::onActivate(const QModelIndex& i)
{
    QString title = i.data().toString();
    QString url = i.data(RadioStationListModel::UrlRole).toString();
    RadioStation rs(url);
    rs.setTitle(title);
    emit startRadio(rs);
}


void
RecentStationsWidget::setModel(QAbstractItemModel* model)
{
    if (m_list->model()) {
        disconnect(m_list->model(), 0, this, 0);
    }
    m_list->setModel(model);
    connect(model, SIGNAL(modelReset()), SLOT(recalcMoreButton()));
    connect(model, SIGNAL(rowsRemoved()), SLOT(recalcMoreButton()));
    connect(model, SIGNAL(rowsInserted()), SLOT(recalcMoreButton()));
}

// re-evaluate the visibility of the "more" button
void
RecentStationsWidget::recalcMoreButton()
{
    bool more = m_list->model() && 
        (m_list->model()->rowCount() > m_list->getDisplayedColumns() * ROW_COUNT);
    m_moreButton->setVisible(more);
}

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
#include <QListWidget>
#include <QPushButton>
#include <QResizeEvent>
#include <QListWidgetItem>
#include <QLabel>
#include <QScrollBar>
#include <lastfm/AuthenticatedUser>
#include <lastfm/XmlQuery>
#include "RecentStationsWidget.h"
#include "PlayableItemWidget.h"

#define GRID_HEIGHT_PX 30


class OurListWidget : public QListWidget
{
protected:

    // switches between 1 and 2 column mode by changing the item grid size
    virtual void resizeEvent(QResizeEvent* e)
    {
        if (e->size().width() < 350) 
            setGridSize(QSize(e->size().width(), GRID_HEIGHT_PX));
        else
            setGridSize(QSize(e->size().width() / 2, GRID_HEIGHT_PX));
    }
};


RecentStationsWidget::RecentStationsWidget()
{
    // todo: start a spinner?
    refresh();
}


void
RecentStationsWidget::refresh()
{
    AuthenticatedUser you;
    connect(you.getRecentStations(), SIGNAL(finished()), SLOT(gotRecentStations()));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(tr("Your Recent Stations")), 0, Qt::AlignCenter);
    layout->addWidget(m_list = new OurListWidget());
    m_list->setWrapping(true);
    m_list->setResizeMode(QListView::Adjust);
    m_list->setUniformItemSizes(false);
    m_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_list->setMaximumHeight(GRID_HEIGHT_PX * 3 + 1);
    layout->addWidget(m_moreButton = new QPushButton(tr("more")), 0, Qt::AlignRight);
    m_moreButton->setObjectName("more");
    connect(m_moreButton, SIGNAL(clicked()), SIGNAL(showMoreRecentStations()));
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

void
RecentStationsWidget::gotRecentStations()
{
    // todo: stop any spinner
    sender()->deleteLater();
    QList<RadioStation> recent = RadioStation::list((QNetworkReply*)sender());

    foreach(const RadioStation& rs, recent) {
        QListWidgetItem* item = new QListWidgetItem(m_list);
        PlayableItemWidget* widget = new PlayableItemWidget(rs.title(), rs);
//        widget->setMinimumHeight(60);
//        widget->setMinimumWidth(100);
        m_list->setItemWidget(item, widget);
        connect(widget, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));
    }
    if (m_list->count() > 6) {
        m_moreButton->setChecked(true);
    }
}

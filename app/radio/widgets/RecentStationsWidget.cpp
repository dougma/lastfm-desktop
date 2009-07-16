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
#include <QResizeEvent>
#include <QListWidgetItem>
#include <QLabel>
#include <lastfm/AuthenticatedUser>
#include <lastfm/XmlQuery>
#include "RecentStationsWidget.h"
#include "PlayableItemWidget.h"

class OurListWidget : public QListWidget
{
protected:
    virtual void resizeEvent(QResizeEvent* e)
    {
        if (e->size().width() < 350) 
            setGridSize(QSize(e->size().width(), 30));
        else
            setGridSize(QSize(e->size().width() / 2, 30));
    }
};

RecentStationsWidget::RecentStationsWidget()
{
    // todo: start a spinner
    refresh();
}

void
RecentStationsWidget::refresh()
{
    AuthenticatedUser you;
    connect(you.getRecentStations(), SIGNAL(finished()), SLOT(gotRecentStations()));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(tr("Your Recent Stations")));
    layout->addWidget(m_list = new OurListWidget());
//    m_list->setFlow(QListView::LeftToRight);
    m_list->setWrapping(true);
    m_list->setResizeMode(QListView::Adjust);
    m_list->setUniformItemSizes(false);
    m_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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
    
}

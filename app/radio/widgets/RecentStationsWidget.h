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

#ifndef RECENT_STATIONS_WIDGET_H
#define RECENT_STATIONS_WIDGET_H

#include "lib/unicorn/StylableWidget.h"
#include <lastfm/RadioStation>

class QPushButton;
class QAbstractItemModel;
class ColumnListView;

class RecentStationsWidget : public StylableWidget
{
    Q_OBJECT;

public:
    RecentStationsWidget();
    void setModel(QAbstractItemModel* model);

signals:
    void startRadio(RadioStation);
    void showMoreRecentStations();

private slots:
    void onActivate(const QModelIndex&);
    void recalcMoreButton();

private:
    ColumnListView* m_list;
    QPushButton* m_moreButton;
};

#endif

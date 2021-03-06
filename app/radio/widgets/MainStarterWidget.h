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

#ifndef MAIN_STARTER_WIDGET_H
#define MAIN_STARTER_WIDGET_H

#include <QWidget>
#include <lastfm/RadioStation>

class QAbstractItemModel;
class RecentStationsWidget;

class MainStarterWidget : public QWidget
{
    Q_OBJECT;

public:
    MainStarterWidget();
    void setRecentStationsModel(QAbstractItemModel*);

signals:
    void startRadio(RadioStation);
    void showMoreRecentStations();
    void combo();
    void yourTags();
    void yourFriends();
    void yourPlaylists();

private:
    RecentStationsWidget* m_recentStationsWidget;
};

#endif

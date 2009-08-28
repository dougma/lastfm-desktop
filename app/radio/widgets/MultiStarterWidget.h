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

#ifndef MULTI_STARTER_WIDGET_H
#define MULTI_STARTER_WIDGET_H

#include "lib/unicorn/StylableWidget.h"
#include <lastfm/RadioStation>
#include <QListWidgetItem>

class QSlider;
class QPushButton;
class SourceListModel;
class SourceListWidget;
class SourceSelectorWidget;

class MultiStarterWidget : public StylableWidget
{
    Q_OBJECT;

public:
    MultiStarterWidget(bool advanced, int maxSources, QWidget *parent = 0);

signals:
    void startRadio(RadioStation);

private slots:
    void onAdd(const QString& item, const QString& imgUrl = QString());
    void onAddItem(QListWidgetItem* item);
    void onPlayClicked();
    void onUserGotTopTags();
    void onUserGotTopArtists();
    void onUserGotFriends();
    void onCheckBox(int checkState);

private:
    SourceListModel* m_sourceModel;
    SourceListWidget* m_sourceList;
    SourceSelectorWidget* m_tags;
    SourceSelectorWidget* m_artists;
    SourceSelectorWidget* m_users;
    QSlider* m_repSlider;
    QSlider* m_mainstrSlider;
    QPushButton* m_playButton;
    const int m_minTagCount;
    const int m_minArtistCount;
};

#endif

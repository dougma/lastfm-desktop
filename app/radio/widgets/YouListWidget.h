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

#ifndef YOU_LIST_WIDGET_H_
#define YOU_LIST_WIDGET_H_

#include <QTreeWidget>

class YouListWidget : public QTreeWidget
{
    Q_OBJECT

public:
    YouListWidget(const QString& username, QWidget* parent = 0);

    QString username() const { return m_username; }

private slots:
    void gotFriends();
    void gotTopArtists();
    void gotTopTags();
    void gotPlaylists();

private:
    QTreeWidgetItem* m_friendsItem;
    QTreeWidgetItem* m_artistsItem;
    QTreeWidgetItem* m_tagsItem;
    QTreeWidgetItem* m_playlistsItem;

    QString m_username;
    bool m_subscriber;
};

#endif // YOU_LIST_WIDGET_H_

/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include <lastfm/global.h>
#include <QListWidget>
#include <QTextEdit>
class QLabel;
 

class MainWidget : public QWidget
{
    Q_OBJECT
    
    struct{
        QListWidget* friends;
        QLabel* friends_count;
        QListWidget* neighbours;
        QLabel* neighbour_tags;
        QTextEdit* me;
        QLabel* scrobbles;
    } ui;

public:
    MainWidget();
    
private slots:
    void onUserGotInfo( QNetworkReply* );
    void onUserGotFriends();
    void onUserGotNeighbours();
    void onUserGotTopTags();
};


class FriendsList : public QListWidget
{
public:
    FriendsList();
};


class NeighboursList : public QListWidget
{
public:
    NeighboursList();
};


class Me : public QTextEdit
{
public:
    Me();
};

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
#ifndef MAIN_WIDGET_H
#define MAIN_WIDGET_H

#include <lastfm/global.h>
#include <lastfm/RadioStation>
#include <QListWidget>
#include <QTextEdit>

#include <QVBoxLayout>
#include <QPushButton>

class QLabel;
class SideBySideLayout;

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
    
signals:
    void startRadio(RadioStation);

private slots:
    void onStartRadio(RadioStation rs);
    void onCombo();
    void onYourTags();
    void onBack();

    void onUserGotInfo( QNetworkReply* );
    void onUserGotFriends();
    void onUserGotNeighbours();
    void onUserGotTopTags();

private:
    SideBySideLayout* m_layout;
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

class BackWrapper : public QWidget
{
    Q_OBJECT;

public:
    BackWrapper(const QString& backButtonLabel, QWidget* child)
    {
        QVBoxLayout* layout = new QVBoxLayout();
        QPushButton* button = new QPushButton(backButtonLabel);
        layout->addWidget(button, 0, Qt::AlignLeft);
        layout->addWidget(child);
        setLayout(layout);
        connect(button, SIGNAL(clicked()), SIGNAL(back()));
    }

signals:
    void back();

};

#endif

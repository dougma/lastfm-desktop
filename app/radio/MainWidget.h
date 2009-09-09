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
#include <QLabel>
#include "PlaylistMeta.h"
#include "RadioStationListModel.h"

class SideBySideLayout;
class NowPlayingState;

class MainWidget : public QWidget
{
    Q_OBJECT
    
public:
    MainWidget( QWidget* parent = 0 );
    
signals:
    void startRadio(RadioStation);

private slots:
    void onStartRadio(RadioStation rs);
    void onShowMoreRecentStations();
    void onCombo();
    void onYourTags();
    void onYourFriends();
    void onYourPlaylists();
    void onBack();
    void onBackDelete();
    void onMoveFinished();
    void onForward();

    void onUserGotFriends();
    void onUserGotTopTags();
    void onUserGotPlaylists();
    void onUserGotRecentStations();

    void onTagActivated(const QModelIndex& idx);
    void onFriendActivated(const QModelIndex& idx);
    void onPlaylistActivated(const QModelIndex& idx);

    void rawrql();

private:
    SideBySideLayout* m_layout;
    NowPlayingState* m_nowPlaying;
    QStringList m_friends;
    QStringList m_tags;
    QList<PlaylistMeta> m_playlists;
    RadioStationListModel m_recentModel;

    QStringList m_rawrqlItems;
};


class NowPlayingState : public QObject
{
    Q_OBJECT

public:
    NowPlayingState(QObject* parent = 0)
        :QObject(parent)
        ,m_playing(false)
    {
    }

    bool isPlaying() const
    {
        return m_playing;
    }

public slots:
    void onTuningIn(RadioStation)
    {
        if (!m_playing) {
            m_playing = true;
            emit playingStarted();
            emit playingStateChange(m_playing);
        }
    }

    void onStopped()
    {
        if (m_playing) {
            m_playing = false;
            emit playingStopped();
            emit playingStateChange(m_playing);
        }
    }

signals:
    void playingStarted();
    void playingStopped();
    void playingStateChange(bool);

private:
    bool m_playing;
};


// BackForwardControls parents a child widget and attaches an optional
// back button at the top-left and an optional forward button at the 
// top-right.
//
class BackForwardControls : public QWidget
{
    Q_OBJECT;

public:

    // if backLabel is an empty string, there is no back button.
    // if nowPlaying is null, there is no now-playing button.

    BackForwardControls(const QString& backLabel, const QString& mainLabel, NowPlayingState* nowPlaying, QWidget* child)
    {
        QVBoxLayout* layout = new QVBoxLayout(this);
        QHBoxLayout* rowLayout = new QHBoxLayout();
        if (!backLabel.isNull()) {
            QPushButton* button = new QPushButton(backLabel);
            button->setObjectName("back");
            connect(button, SIGNAL(clicked()), SIGNAL(back()));
            rowLayout->addWidget(button, 1, Qt::AlignLeft);
        }
        if (!mainLabel.isNull()) {
            rowLayout->addWidget(new QLabel(mainLabel), 1, Qt::AlignCenter);
        }
        if (nowPlaying) {
            // a button which toggles enable/disabled state (use css to make it visible/invisible)
            QPushButton* button = new QPushButton(tr("Now Playing"));
            button->setObjectName("NowPlayingButton");
            button->setEnabled(nowPlaying->isPlaying());
            connect(button, SIGNAL(clicked()), SIGNAL(forward()));
            connect(nowPlaying, SIGNAL(playingStateChange(bool)), button, SLOT(setEnabled(bool)));
            rowLayout->addWidget(button, 1, Qt::AlignRight);
        } else {
            rowLayout->addStretch(1);            // need this to get the label centered
        }
        if (!backLabel.isNull() || nowPlaying) 
            layout->addLayout(rowLayout);
        layout->addWidget(child);
    }

signals:
    void back();
    void forward();
};

#endif

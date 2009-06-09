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
#ifndef PLAYLIST_WIDGET_H_
#define PLAYLIST_WIDGET_H_

#include <QTreeView>
#include "PlaylistModel.h"
#include "playdar/PlaydarConnection.h"
#include "playdar/BoffinTagRequest.h"
#include "TrackSource.h"

class PlaylistWidget: public QTreeView {

    Q_OBJECT

public:
	PlaylistWidget(PlaydarConnection* playdar, QWidget* p = 0) :
		QTreeView(p), m_playdar(playdar) {
		setModel(&m_model);
		setAlternatingRowColors(true);
		connect(this, SIGNAL( doubleClicked(QModelIndex)),
				SLOT( onDoubleClicked(QModelIndex)));
	}

public slots:
	void loadFromRql(QString rql) {
	}

signals:
	void play(const QUrl&);

private slots:
	void onDoubleClicked(const QModelIndex& index) {
		qDebug() << "Play: "
				<< index.data(PlaylistModel::UrlRole).toUrl().toString();
		emit play(index.data(PlaylistModel::UrlRole).toUrl());
	}

private:
	PlaydarConnection* m_playdar;
	PlaylistModel m_model;
};

#endif //PLAYLIST_WIDGET_H_

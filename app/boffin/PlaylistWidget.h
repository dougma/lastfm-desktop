#ifndef PLAYLIST_WIDGET_H_
#define PLAYLIST_WIDGET_H_

#include <QTreeView>
#include "PlaylistModel.h"
#include "PlaydarConnection.h"
#include "BoffinTagRequest.h"
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
		BoffinRqlRequest* req = m_playdar->boffinRql(rql);
		TrackSource* source = new TrackSource(req);
		connect(req, SIGNAL(playableItem(BoffinPlayableItem)), source,
				SLOT(onPlayableItem(BoffinPlayableItem)));
		connect(source, SIGNAL(ready( Track)), model(),
				SLOT(addTrack(Track)));
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

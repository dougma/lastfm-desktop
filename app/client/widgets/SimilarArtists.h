/***************************************************************************
*   Copyright 2005-2008 Last.fm Ltd.                                      *
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
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
***************************************************************************/

#ifndef SIMILAR_ARTISTS_H
#define SIMILAR_ARTISTS_H

#include <QAbstractItemModel>
#include <QList>
#include <QVariant>
#include <QListView>
#include <QPixmap>


class SimilarArtists : public QListView
{
	Q_OBJECT

	class SimilarArtistsModel *m_model;

public:
	SimilarArtists();
	void clear();
	void setContent(const class CoreDomElement &);

private slots:
	void onDoubleClicked(const QModelIndex &index);

};


class SimilarArtistsModel : public QAbstractItemModel
{
	Q_OBJECT

	virtual QModelIndex index( int row, int column, const QModelIndex& ) const { return createIndex( row, column ); }
	virtual QModelIndex parent(const QModelIndex& ) const { return QModelIndex(); }
	virtual int rowCount( const QModelIndex& ) const { return m_items.count(); }
	virtual int columnCount( const QModelIndex& ) const { return 1; }
	virtual Qt::ItemFlags flags( const QModelIndex& ) const { return Qt::ItemIsDragEnabled | Qt::ItemIsEnabled | Qt::ItemIsSelectable; }

	QList<class SimilarArtistsItem *> m_items;
	class WsAccessManager *m_wam;

public:
	SimilarArtistsModel(QObject *parent = 0);
	~SimilarArtistsModel();

	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual QMimeData* mimeData( const QModelIndex &index ) const;
	virtual QMimeData* mimeData( const QModelIndexList &indexes ) const;

	void clear();
	void setContent(const class CoreDomElement &);

private slots:
	void onImageAvailable();
};


class SimilarArtistsItem : public QObject
{
	Q_OBJECT

	QString m_artist;
	QPixmap m_image;
	float m_weighting;

public:
	SimilarArtistsItem( const class CoreDomElement&, class WsAccessManager * );

	QString artist() const { return m_artist; }
	QPixmap image() const { return m_image; }
	float weighting() const { return m_weighting; }
	
	QMimeData* mimeData() const;

signals:
	void imageAvailable();

private slots:
	void onImageDownloaded();
};

#endif

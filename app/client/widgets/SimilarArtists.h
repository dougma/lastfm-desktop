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

#ifndef SIMILARARTISTS_H
#define SIMILARARTISTS_H

#include <QAbstractItemModel>
#include <QList>
#include <QVariant>
#include <QWidget>
#include <QPixmap>


class SimilarArtists : public QWidget
{
	Q_OBJECT

	class SimilarArtistsModel *m_model;

public:
	SimilarArtists();
	void clear();
	void setContent(const class CoreDomElement &);

private slots:
	void onDoubleClicked(const QModelIndex &index);

signals:
	void artistDoubleClicked(QString);
};


class SimilarArtistsModel : public QAbstractItemModel
{
	Q_OBJECT

	virtual QModelIndex index( int row, int column, const QModelIndex& ) const { return createIndex( row, column ); }
	virtual QModelIndex parent(const QModelIndex& ) const { return QModelIndex(); }
	virtual int rowCount( const QModelIndex& ) const { return m_items.count(); }
	virtual int columnCount( const QModelIndex& ) const { return 1; }

	QList<class SimilarArtistsItem *> m_items;
	class WsAccessManager *m_wam;

public:
	SimilarArtistsModel(QObject *parent = 0);
	~SimilarArtistsModel();

	virtual QVariant data(const QModelIndex &index, int role) const;

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
	float m_matchPercent;

public:
	SimilarArtistsItem( const class CoreDomElement&, class WsAccessManager * );

	QString artist() const { return m_artist; }
	QPixmap image() const { return m_image; }
	float matchPercent() const { return m_matchPercent; }

signals:
	void imageAvailable();

private slots:
	void onImageDownloaded();
};

#endif

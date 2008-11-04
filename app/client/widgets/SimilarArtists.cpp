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

#include "SimilarArtists.h"
#include "FirehoseDelegate.h"
#include "widgets/UnicornWidget.h"
#include "widgets/UnicornTabWidget.h"
#include "PlayableMimeData.h"
#include "app/moose.h"
#include "lib/lastfm/radio/RadioStation.h"
#include "lib/lastfm/core/CoreDomElement.h"
#include "lib/lastfm/ws/WsAccessManager.h"
#include "the/mainWindow.h"
#include "radio/buckets/DelegateDragHint.h"
#include "radio/buckets/PrimaryBucket.h"
#include "radio/buckets/PlayerBucket.h"
#include <QDebug>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QtGui>



SimilarArtists::SimilarArtists()
{
	setModel( m_model = new SimilarArtistsModel( this ) );

	delete itemDelegate();
	setItemDelegate( new FirehoseDelegate );
	setAttribute( Qt::WA_MacShowFocusRect, false );

	QPalette p = palette();    
	p.setBrush( QPalette::Text, Qt::white );
	
	m_model->setSupportedDragActions( Qt::CopyAction | Qt::MoveAction );

	setPalette( p );
	setAutoFillBackground( true );
	setDragEnabled( true );
	setDragDropMode( QAbstractItemView::DragOnly );
	setMovement( QListView::Free );

	connect( this, SIGNAL(doubleClicked( QModelIndex )), SLOT(onDoubleClicked( QModelIndex )) );
}


void
SimilarArtists::onDoubleClicked(const QModelIndex &index)
{
    QStyleOptionViewItem options;
    options.initFrom( this );
    options.rect = visualRect( index );
    DelegateDragHint* dragHint = new DelegateDragHint( itemDelegate(), index, options, this );
    dragHint->setMimeData( model()->mimeData( QModelIndexList() << index ) );
    
    //FIXME: This is soo incredibly unencapsulated! (applies to SimilarArtists, TagListWidget and FirehoseView )
    dragHint->dragTo( The::mainWindow().ui.primaryBucket->ui.playerBucketWidget );
}


void
SimilarArtists::clear()
{
	m_model->clear();
}


void 
SimilarArtists::setContent(const class CoreDomElement &e)
{
	m_model->setContent( e );
}

///////////////////////////////////////////


SimilarArtistsModel::SimilarArtistsModel(QObject *parent)
                   : QAbstractItemModel(parent)
                   , m_wam(0)
{}


SimilarArtistsModel::~SimilarArtistsModel()
{
	delete m_wam;
}


void
SimilarArtistsModel::clear()
{
	// destroying WsAccessManager destroys outstanding 
	// requests and child SimilarArtistsItems objects
	delete m_wam;	
	m_wam = new WsAccessManager;

	if (!m_items.isEmpty())	{
		beginRemoveRows(QModelIndex(), 0, m_items.count() - 1 );
		m_items.clear();
		endRemoveRows();
	}
}


void
SimilarArtistsModel::setContent(const CoreDomElement &lfm)
{
	clear();

	QList<CoreDomElement> artists = lfm.optional("similarartists").children("artist");

	if (!artists.isEmpty()) {
		int limit = 20;
		beginInsertRows(QModelIndex(), 0, qMin( limit, artists.count()) - 1 );
		foreach (CoreDomElement a, artists) {
			SimilarArtistsItem *item = new SimilarArtistsItem(a, m_wam);
			connect( item, SIGNAL(imageAvailable()), SLOT(onImageAvailable()) );
			m_items.append(item);
			item->setParent(m_wam);

			if (0 == --limit) break;
		}
		endInsertRows();
	}
}


void
SimilarArtistsModel::onImageAvailable()
{
	int row = m_items.indexOf( static_cast<SimilarArtistsItem *>(sender()) );
	if (row >= 0 && row < m_items.count()) {
		//emit dataChanged( createIndex(row, 1), createIndex(row, 1) );	// doesn't seem to work
		emit layoutChanged();
	}
}


QVariant
SimilarArtistsModel::data(const QModelIndex &index, int role) const
{   
	int const row = index.row();

	if (!index.isValid()) return QVariant();
	if (row < 0 || row >= m_items.count()) return QVariant();

	switch (role)
	{
        case Qt::DisplayRole: return m_items[row]->artist();
        case Qt::DecorationRole: return m_items[row]->image();
        case moose::WeightingRole: return m_items[row]->weighting();
	}

	return QVariant();
}


QMimeData* 
SimilarArtistsModel::mimeData( const QModelIndex &index ) const
{
	int const row = index.row();
	if( !index.isValid()) return 0;
	if( row < 0 || row >= m_items.count()) return 0;
	
	return m_items[row]->mimeData();
}


QMimeData* 
SimilarArtistsModel::mimeData( const QModelIndexList &indexes ) const
{
	//FIXME: only single items are supported at the moment so multiselection
    //		 drag and drop will only drag the first item :(
	if (indexes.isEmpty()) return 0;
	
	return mimeData( indexes.first() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////


SimilarArtistsItem::SimilarArtistsItem( const class CoreDomElement& artist, WsAccessManager *wam )
{    
	m_artist = artist["name"].text();
	m_weighting = artist["match"].text().toDouble() / 100;

	QString imgUrl(artist["image size=medium"].text());
	QNetworkReply* reply = wam->get( QNetworkRequest(imgUrl) );
	connect(reply, SIGNAL(finished()), this, SLOT(onImageDownloaded()));
}


void
SimilarArtistsItem::onImageDownloaded()
{
	QByteArray data = static_cast<QNetworkReply*>(sender())->readAll();
	m_image.loadFromData( data );
	if (m_image.isNull())
		m_image = QPixmap( ":/lastfm/no/artist.png" );		// todo: is this the right image to use?
	emit imageAvailable();
}

QMimeData* 
SimilarArtistsItem::mimeData() const
{ 
	PlayableMimeData* mimeData = PlayableMimeData::createFromArtist( artist() );
	
	if( !image().isNull() )
		mimeData->setImageData( m_image.toImage() ); //In Qt 4.4.1 passing setImageData( QPixmap ) crashes so convert to QImage.
	
	return mimeData;
}

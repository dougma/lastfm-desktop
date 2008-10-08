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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include "PlayerBucket.h"
#include "widgets/ImageButton.h"
#include "lib/lastfm/ws/WsAccessManager.h"
#include <QListView>
#include <QMenu>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStyledItemDelegate>
#include <QVBoxLayout>

const QString PlayerBucket::k_dropText  = tr( "Drag something in here to play it." );


PlayerBucket::PlayerBucket( QWidget* w )
			 :QListWidget( w ),
			  m_showDropText( true)
{
	ui.previewList = new QListWidget;
	ui.previewList->show();
	
	m_networkManager = new WsAccessManager( this );
	
	setItemDelegate( new PlayerBucketDelegate( this ));
	setAcceptDrops( true );
	setSelectionMode( QAbstractItemView::ExtendedSelection );
	setContextMenuPolicy( Qt::CustomContextMenu );
	setDropIndicatorShown( false );
	setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	
}


void
PlayerBucket::paintEvent( QPaintEvent* event )
{
	QAbstractItemModel* itemModel = model();
	if( !itemModel->rowCount() )
	{
		QPainter p( viewport() );
		p.setRenderHint( QPainter::Antialiasing, true );
		QPen pen( QColor( 0x2e, 0x2e, 0x2e, 0x99 ), 3, Qt::DashLine, Qt::RoundCap );
		pen.setDashPattern( QVector<qreal>() << 5 << 2 );
		p.setPen( pen );
		p.drawRoundedRect( viewport()->rect().adjusted( 20, 20, -20, -20), 10, 10 );

		QFont dropFont = p.font();
		dropFont.setPointSize( 20 );
		p.setFont( dropFont );

		int x = (viewport()->rect().width() / 2) - ( p.fontMetrics().width( k_dropText ) / 2);
		int y = (viewport()->rect().height() / 2) - ( p.fontMetrics().height() / 2);
		p.drawText( viewport()->rect().adjusted( 25, 25, -25, -25), Qt::AlignCenter | Qt::TextWordWrap, k_dropText );
		return;
	}
	
	QPainter p( viewport() );
	p.setClipRect( event->rect());
	
	QRect rect = viewport()->rect();
	
	int iconRowCount = ( itemModel->rowCount() * 75 ) / rect.width();
	iconRowCount++;
	int iconColumnCount = itemModel->rowCount() / iconRowCount;
	if( itemModel->rowCount() % iconRowCount )
		iconColumnCount++;
	
	int delegatesX = (rect.width() / 2 ) - ((iconColumnCount * 75) / 2 );
	int delegatesY = (rect.height() / 2 ) - ((iconRowCount * 75) / 2);
	QRect itemRect( delegatesX, delegatesY, 75, 75 );
	
	int index = 0;
	for( int row = 0; row < iconRowCount; row++ )
	{
		for( int col = 0; col < iconColumnCount && index < itemModel->rowCount(); col++ )
		{
			QModelIndex i = itemModel->index( index, 0 );
			QStyledItemDelegate* delegate = static_cast<QStyledItemDelegate*>( itemDelegate( i ));
			QStyleOptionViewItem styleOptions;
			styleOptions.rect = itemRect;

			if( currentIndex() == i )
				styleOptions.state = QStyle::State_Active;
			
			styleOptions.rect.translate( itemRect.width() * col, itemRect.height() * row );
			m_itemRects[ i ] = styleOptions.rect;
			
			delegate->paint( &p, styleOptions, i );
			index++;

		}
	}
}


void 
PlayerBucket::resizeEvent ( QResizeEvent* event )
{
	viewport()->setBackgroundRole( QPalette::Window );
	QLinearGradient lg( viewport()->rect().topLeft(), viewport()->rect().bottomLeft());
	lg.setColorAt( 0, Qt::black );
	lg.setColorAt( 1, QColor( 0x20, 0x20, 0x20 ));
	
	QPalette p;
	p.setBrush( QPalette::Window, lg );
	viewport()->setPalette( p );
}


void 
PlayerBucket::dropEvent( QDropEvent* event)
{ 
	QListWidgetItem* item = new QListWidgetItem;
	item->setForeground( Qt::white );
	item->setBackground( QColor( 0x2e, 0x2e, 0x2e));
	item->setText( event->mimeData()->text() );
	item->setIcon( QIcon( QPixmap::fromImage( event->mimeData()->imageData().value<QImage>())) );
	item->setFlags( item->flags() ^ Qt::ItemIsDragEnabled );
	addItem( item );
	
	QString query = "user:" + model()->index( 0, 0 ).data( Qt::DisplayRole ).toString();
	for( int i = 1; i < model()->rowCount(); i++ )
	{
		QModelIndex index = model()->index( i, 0 );
		query += " or user:" + index.data( Qt::DisplayRole ).toString();
	}
	qDebug() << "RadioQL query: " << query;
	QNetworkReply* reply = m_networkManager->get( QNetworkRequest( "http://tester:futureofmusic@ws.jono.dev.last.fm:8090/radioTest.php?query=" + query ));
	connect( reply, SIGNAL( finished()), SLOT( playlistFetched()) );
	
	event->acceptProposedAction();
}


void
PlayerBucket::playlistFetched()
{
	QNetworkReply* reply = static_cast< QNetworkReply* > (sender());
	
	QDomDocument xmlDoc;
	QByteArray data = reply->readAll();
	xmlDoc.setContent( data );
	
	QDomNodeList tracks = xmlDoc.elementsByTagName( "track" );

	ui.previewList->clear();
	for( int i = 0; i < tracks.size(); i++ )
	{
		QString info;
		QDomElement track = tracks.at( i ).toElement();
		info += track.elementsByTagName( "creator" ).at( 0 ).toElement().text();
		info += " - " + track.elementsByTagName( "title" ).at( 0 ).toElement().text();
		ui.previewList->addItem( info );
	}
}


void 
PlayerBucket::dragEnterEvent ( QDragEnterEvent * event )
{		
	event->accept( rect() );
}


QModelIndex 
PlayerBucket::indexAt( const QPoint& point ) const
{
	foreach ( const QModelIndex& i, m_itemRects.keys() )
	{
		const QRect& r = m_itemRects.value(i);
		if( r.contains( point ))
		{
			return i;
		}
	}
	return QModelIndex();
}


QRect 
PlayerBucket::visualRect ( const QModelIndex & index ) const 
{
	QAbstractItemModel* itemModel = model();
	
	if( !itemModel->rowCount())
	{
		return viewport()->rect();
	}
	
	QRect rect = viewport()->rect();
	int iconRowCount = ( itemModel->rowCount() * 75 ) / rect.width();
	iconRowCount++;
	int iconColumnCount = itemModel->rowCount() / iconRowCount;

	if ( itemModel->rowCount() % iconColumnCount )
		iconRowCount++;
	
	
	int delegatesX = (rect.width() / 2 ) - (((iconColumnCount + iconRowCount - 1) * 75) / 2 );
	int delegatesY = (rect.height() / 2 ) - ((iconRowCount * 75) / 2);
	
	return QRect( delegatesX, delegatesY, iconColumnCount, iconRowCount );
}

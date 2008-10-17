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
#include "PrimaryBucket.h"
#include "PlayableListItem.h"
#include "PlayableMimeData.h"
#include "the/radio.h"
#include "lib/lastfm/ws/WsAccessManager.h"
#include <QListView>
#include <QMenu>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStyledItemDelegate>
#include <QVBoxLayout>
#include <QScrollBar>

const QString PlayerBucket::k_dropText  = tr( "Drag something in here to play it." );
const int PlayerBucket::k_itemMargin = 4;

//These should be based on the delegate's sizeHint - this requires
//the delegate to calculate the sizeHint correctly however and this 
//is not currently done!
const int PlayerBucket::k_itemSizeX = 75;
const int PlayerBucket::k_itemSizeY = 75;


PlayerBucket::PlayerBucket( QWidget* w )
			 :QListWidget( w ),
			  m_showDropText( true)
{
	ui.previewList = new QListWidget;
    ui.previewList->hide();
	m_networkManager = new WsAccessManager( this );
	
	setItemDelegate( new PlayerBucketDelegate( this ));
	setAcceptDrops( true );
    setDragDropMode( QAbstractItemView::DragDrop );
    setDropIndicatorShown( false );

	setSelectionMode( QAbstractItemView::ExtendedSelection );
	setContextMenuPolicy( Qt::CustomContextMenu );
    setAutoFillBackground( true );
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

		p.drawText( viewport()->rect().adjusted( 25, 25, -25, -25), Qt::AlignCenter | Qt::TextWordWrap, k_dropText );
		return;
	}
	
	QPainter p( viewport() );
	p.setClipRect( event->rect());
	
	foreach ( const QModelIndex& i, m_itemRects.keys() )
	{
		const QRect& r = m_itemRects.value(i);
       
        QStyledItemDelegate* delegate = static_cast<QStyledItemDelegate*>( itemDelegate( i ));
        QStyleOptionViewItem styleOptions;
        styleOptions.rect = r;
        
        if( currentIndex() == i )
            styleOptions.state = QStyle::State_Active;

        delegate->paint( &p, styleOptions, i );
    }
}


void 
PlayerBucket::scrollContentsBy( int dx, int dy )
{
    calculateLayout();
    viewport()->update();
}


void 
PlayerBucket::resizeEvent ( QResizeEvent* event )
{
	Q_UNUSED( event );
	viewport()->setBackgroundRole( QPalette::Window );
	QLinearGradient lg( viewport()->rect().topLeft(), viewport()->rect().bottomLeft());
	lg.setColorAt( 0, Qt::black );
	lg.setColorAt( 1, QColor( 0x20, 0x20, 0x20 ));
	
	QPalette p;
	p.setBrush( QPalette::Window, lg );
	viewport()->setPalette( p );
    
    calculateLayout();
    
}


void 
PlayerBucket::calculateLayout()
{
    QAbstractItemModel* itemModel = model();
    
    QRect rect = viewport()->rect();
	
    if( rect.width() < 1 || rect.height() < 1 )
        return;

	int iconRowCount = ( itemModel->rowCount() * (k_itemSizeX + k_itemMargin) ) / rect.width();
	iconRowCount++;
	int iconColumnCount = itemModel->rowCount() / iconRowCount;
	if( itemModel->rowCount() % iconRowCount )
		iconColumnCount++;
	
	int delegatesX = (rect.width() / 2 ) - ((iconColumnCount * (k_itemSizeX + k_itemMargin )) / 2 );
	int delegatesY = (rect.height() / 2 ) - ((iconRowCount * ( k_itemSizeY + k_itemMargin)) / 2);

    delegatesY -= verticalScrollBar()->value();
    
    QRect itemRect( delegatesX, delegatesY, k_itemSizeX, k_itemSizeY );
	
	int index = 0;
	for( int row = 0; row < iconRowCount; row++ )
	{
		for( int col = 0; col < iconColumnCount && index < itemModel->rowCount(); col++ )
		{
			QModelIndex i = itemModel->index( index, 0 );
            
            QRect rect = itemRect;
			
			rect.translate( (itemRect.width() + k_itemMargin ) * col, (itemRect.height() + k_itemMargin) * row );
			m_itemRects[ i ] = rect;
			
			index++;
            
		}
	}
    
    verticalScrollBar()->setPageStep( (iconRowCount * itemRect.height()) );
    verticalScrollBar()->setRange( 0, (iconRowCount * itemRect.height()) - viewport()->size().height());
}


void 
PlayerBucket::dropEvent( QDropEvent* event)
{
	if( !event->mimeData() )
		return;
	
	if( addFromMimeData( event->mimeData() ) )
		event->acceptProposedAction();
	else
		event->ignore();
}


//FIXME: Don't allow duplicate items?!
//       on second thoughts.. there may be cases when duplicate items are acceptable: ( jonocole and mxcl ) or (jonocole and not jazz )
//       probably fairly advanced though - but either way this needs decisions to be made.
bool 
PlayerBucket::addFromMimeData( const QMimeData* d )
{
	const PlayableMimeData* data = qobject_cast< const PlayableMimeData* >( d );
	if( !data )
		return false;
	
	
	PlayableListItem* item = PlayableListItem::createFromMimeData( data, this );
	item->setForeground( Qt::white );
	item->setBackground( QColor( 0x2e, 0x2e, 0x2e));
	item->setFlags( item->flags() ^ Qt::ItemIsDragEnabled );

	calculateLayout();	
    
	//Send query
	QString query = queryString( model()->index( 0, 0 ), false );
	for( int i = 1; i < model()->rowCount(); i++ )
	{
		QModelIndex index = model()->index( i, 0 );
		query += queryString( index );
	}
	qDebug() << "RadioQL query: " << query;
	QNetworkReply* reply = m_networkManager->get( QNetworkRequest( "http://tester:futureofmusic@ws.jono.dev.last.fm:8090/radioTest.php?query=" + query ));
	connect( reply, SIGNAL( finished()), SLOT( playlistFetched()) );

	return true;
}


QString 
PlayerBucket::queryString( const QModelIndex i, bool joined ) const 
{  
	QString qs;
	
	switch ( i.data( Qt::UserRole ).toInt() ) {
		case PlayableMimeData::UserType:
            if( joined )
                qs = " or ";
			qs += "user:";
			break;
		case PlayableMimeData::ArtistType:
            if( joined )
                qs = " and ";
			qs += "simart:";
			break;
		case PlayableMimeData::TagType:
            if( joined )
                qs = " and ";
			qs += "tag:";
			break;
	}
	qs += "" + i.data( Qt::DisplayRole ).toString() + "";
	return qs;
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
	event->accept( rect());
}


QModelIndex 
PlayerBucket::indexAt( const QPoint& point ) const
{
	foreach ( const QModelIndex& i, m_itemRects.keys() )
	{
		const QRect& r = m_itemRects.value(i);
		if( r.contains( point ))
		{
            //For some reason the viewport isn't always repainted
            //when the first item in the list is clicked - this works
            //around the issue.
            viewport()->update();
			return i;
		}
	}
	return QModelIndex();
}


QRect 
PlayerBucket::visualRect ( const QModelIndex & index ) const 
{
	Q_UNUSED( index );
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


void
PlayerBucket::play()
{
    QString query = queryString( model()->index( 0, 0 ), false );
	for( int i = 1; i < model()->rowCount(); i++ )
	{
		QModelIndex index = model()->index( i, 0 );
		query += queryString( index );
	}
    
    qDebug() << "Playing query radio";
    The::radio().play( RadioStation::rql( query) );
}
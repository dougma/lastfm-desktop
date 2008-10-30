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
#include "SeedDelegate.h"
#include "PlayableListItem.h"
#include "PlayableMimeData.h"
#include "the/radio.h"
#include "lib/lastfm/ws/WsAccessManager.h"
#include <QListView>
#include <QScrollBar>
#include <QPushButton>
#include "widgets/ImageButton.h"

Q_DECLARE_METATYPE( PlayableListItem* )

const QString PlayerBucket::k_dropText  = tr( "Drag something in here to play it." );
const int PlayerBucket::k_itemMargin = 4;

//These should be based on the delegate's sizeHint - this requires
//the delegate to calculate the sizeHint correctly however and this 
//is not currently done!
const int PlayerBucket::k_itemSizeX = 66;
const int PlayerBucket::k_itemSizeY = 88;


PlayerBucket::PlayerBucket( QWidget* w )
			 :QListWidget( w ),
			  m_showDropText( true)
{
    connect( this, SIGNAL( currentItemChanged(QListWidgetItem*, QListWidgetItem*)), SLOT( onCurrentItemChanged(QListWidgetItem*, QListWidgetItem*)));
    setIconSize( QSize( 66, 68 ));
    m_networkManager = new WsAccessManager( this );
    
    ui.clearButton = new ImageButton( ":buckets/radio_clear_all_x.png", this );
    ui.clearButton->resize( ui.clearButton->size());
    connect( ui.clearButton, SIGNAL( clicked()), SLOT( clearItems()));
    
    ui.removeButton = new ImageButton( ":buckets/x_button.png", this );
    ui.removeButton->hide();
    connect( ui.removeButton, SIGNAL( clicked()), SLOT( removeCurrentItem()));
    
    ui.queryEditButton = new ImageButton( ":buckets/show_query.png", this );
   
    //Not sure why this is needed but otherwise the button isn't sized properly :-s
    ui.queryEditButton->resize( ui.queryEditButton->size());
    connect( ui.queryEditButton, SIGNAL( clicked()), SLOT( showQuery()));
    
    ui.queryEdit = new QLineEdit( this );
    ui.queryEdit->setAttribute( Qt::WA_MacShowFocusRect, false );
    QPalette p = ui.queryEdit->palette();
    p.setBrush( QPalette::Base, Qt::transparent );
    p.setBrush( QPalette::Text, Qt::white );
    ui.queryEdit->setPalette( p);
    ui.queryEdit->setAlignment( Qt::AlignCenter );
    ui.queryEdit->setFrame( false );
    
    connect( ui.queryEdit, SIGNAL( returnPressed()), SLOT( onQueryEditReturn()));
    
    
    setAttribute( Qt::WA_MacShowFocusRect, false );
    
	setItemDelegate( new SeedDelegate( this ));
	setAcceptDrops( true );
    setDragDropMode( QAbstractItemView::DragDrop );
    setDropIndicatorShown( false );

	setSelectionMode( QAbstractItemView::ExtendedSelection );
	setContextMenuPolicy( Qt::CustomContextMenu );
    setAutoFillBackground( true );
    calculateLayout();
}


void
PlayerBucket::paintEvent( QPaintEvent* event )
{
    QPainter p( viewport() );
	p.setClipRect( event->rect());
    p.setRenderHint( QPainter::Antialiasing, true );
    QPen pen( QColor( 0x4e, 0x4e, 0x4e ), 3, Qt::DashLine, Qt::RoundCap );
    pen.setDashPattern( QVector<qreal>() << 5 << 2 );
    pen.setWidth( 1 );
    p.setPen( pen );
    p.drawRoundedRect( viewport()->rect().adjusted( 5, 5, -5, -5), 10, 10 );
    
	QAbstractItemModel* itemModel = model();
	if( !itemModel->rowCount() )
	{
		QFont dropFont = p.font();
		dropFont.setPointSize( 20 );
		p.setFont( dropFont );

		p.drawText( viewport()->rect().adjusted( 25, 25, -25, -25), Qt::AlignCenter | Qt::TextWordWrap, k_dropText );
		return;
	}
	
    //draw delegate items
	foreach ( const QModelIndex& i, m_itemRects.keys() )
	{
		const QRect& r = m_itemRects.value(i);

        QAbstractItemDelegate* delegate = static_cast<QAbstractItemDelegate*>( itemDelegate( i ));
        QStyleOptionViewItem styleOptions;
        styleOptions.rect = r;
        
        if( i == currentIndex())
            styleOptions.state = QStyle::State_Selected;
        
        delegate->paint( &p, styleOptions, i );
        
    }

}


void 
PlayerBucket::scrollContentsBy( int dx, int dy )
{
    Q_UNUSED( dx ); Q_UNUSED( dy );
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
	lg.setColorAt( 1, QColor( 0x17, 0x17, 0x17 ));
	
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
	QRect itemDeleteRect( itemRect.topRight().x() - 9, itemRect.topRight().y() - 9, 18, 18 );
    
    m_itemRects.clear();
	int index = 0;
	for( int row = 0; row < iconRowCount; row++ )
	{
		for( int col = 0; col < iconColumnCount && index < itemModel->rowCount(); col++ )
		{
			QModelIndex i = itemModel->index( index, 0 );
            
            QRect rect = itemRect;
            QRect deleteRect = itemDeleteRect;
		
            QPoint offset( (itemRect.width() + k_itemMargin ) * col, (itemRect.height() + k_itemMargin) * row );
			rect.translate( offset );
            deleteRect.translate( offset );
            
			m_itemRects[ i ] = rect;
			
			index++;
            
		}
	}
    
    verticalScrollBar()->setPageStep( (iconRowCount * itemRect.height()) );
    verticalScrollBar()->setRange( 0, (iconRowCount * itemRect.height()) - viewport()->size().height());
    
    calculateItemRemoveLayout();
    calculateToolIconsLayout();
    
    //update the query inputbox
    ui.queryEdit->setText( queryString());
}


void 
PlayerBucket::calculateToolIconsLayout()
{
    //show/hide clear button

    if( model()->rowCount() <= 0 )
    {
        ui.clearButton->hide();
        ui.queryEdit->hide();
        ui.queryEditButton->hide();
        return;
    }
    
    ui.clearButton->move( rect().translated( -66, 2 ).topRight());
    ui.clearButton->show();
    if( !ui.queryEdit->isVisible())
    {
        ui.queryEditButton->move( rect().translated( -32, -ui.queryEditButton->height()-2).bottomLeft());
        ui.queryEditButton->show();
    }
    else
    {
        ui.queryEdit->move( rect().bottomLeft() - QPoint( -8, ui.queryEdit->height() + 8 ));   
        ui.queryEdit->resize( (size().width() - 16), ui.queryEdit->rect().height() );
    }

}


void 
PlayerBucket::calculateItemRemoveLayout()
{
    //move the remove button to correct position
    if( currentIndex().isValid() )
    {
        QRect curRect = m_itemRects[ currentIndex() ];
        ui.removeButton->show();
        ui.removeButton->move( curRect.topRight() - QPoint( ui.removeButton->width() +2,  -4 ));
    }
    else
        ui.removeButton->hide();   
}


void 
PlayerBucket::dropEvent( QDropEvent* event)
{
	if( !event->mimeData() )
		return;
	
	if( addFromMimeData( event->mimeData()))
		event->acceptProposedAction();
	else
		event->ignore();
}


bool 
PlayerBucket::addFromMimeData( const QMimeData* d )
{
	const PlayableMimeData* data = qobject_cast< const PlayableMimeData* >( d );
	if( !data )
		return false;
	
	PlayableListItem* item = PlayableListItem::createFromMimeData( data );
	item->setForeground( Qt::white );
	item->setBackground( QColor( 0x2e, 0x2e, 0x2e));
	item->setFlags( item->flags() ^ Qt::ItemIsDragEnabled );
    
    if( !addItem( item ) )
        return false;

	calculateLayout();	
    
	//Send query
	QString query = queryString( model()->index( 0, 0 ), false );
	for( int i = 1; i < model()->rowCount(); i++ )
	{
		QModelIndex index = model()->index( i, 0 );
		query += queryString( index );
	}

	return true;
}
                          
                          
                          
                          
QString 
PlayerBucket::queryString() const
{
    QString query = queryString( model()->index( 0, 0 ), false );
    for( int i = 1; i < model()->rowCount(); i++ )
    {
        QModelIndex index = model()->index( i, 0 );
        query += queryString( index );
    }
    return query;
}


QString 
PlayerBucket::queryString( const QModelIndex i, bool joined ) const 
{  
	QString qs;
	
	switch ( i.data( moose::TypeRole ).toInt() ) {
		case Seed::UserType:
            if( joined )
                qs = " and ";
			qs += "user:";
			break;
		case Seed::ArtistType:
            if( joined )
                qs = " and ";
			qs += "simart:";
			break;
		case Seed::TagType:
            if( joined )
                qs = " and ";
			qs += "tag:";
			break;
	}
	qs += "" + i.data( Qt::DisplayRole ).toString() + "";
	return qs;
}


void 
PlayerBucket::dragEnterEvent ( QDragEnterEvent * event )
{		
	event->accept();
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
    
    RadioStation station = RadioStation::rql( queryString() );
    
    station.setTitle( "" );
    The::radio().play( station );
}


void 
PlayerBucket::addAndLoadItem( const QString& itemText, const Seed::Type type )
{
    PlayableListItem* item = new PlayableListItem;
    item->setText( itemText );
    item->setType( type );
	item->setForeground( Qt::white );
	item->setBackground( QColor( 0x2e, 0x2e, 0x2e));
	item->setFlags( item->flags() ^ Qt::ItemIsDragEnabled );
    item->fetchImage();
    if( addItem( item ) )
        calculateLayout();
    
}


bool 
PlayerBucket::addItem( PlayableListItem* item )
{
    PlayableListItem* foundItem = 0;
    foreach( QListWidgetItem* anItem, findItems( item->text(), Qt::MatchFixedString ))
    {
        PlayableListItem* pItem;
        if( !( pItem= dynamic_cast<PlayableListItem*>( anItem )))
            continue;
        
        if( pItem->playableType() == item->playableType() )
        {
            foundItem = pItem;
            break;
        }
    }
    
    if( foundItem )
    {
        foundItem->flash();
        return false;
    }
    
    addItem( (QListWidgetItem*)item );
    return true;
}


void
PlayerBucket::addItem( QListWidgetItem* item )
{
    QListWidget::addItem( item );
}


void 
PlayerBucket::removeIndex( const QModelIndex& index )
{
    PlayableListItem* item = static_cast<PlayableListItem*>( itemFromIndex( index ));
    if( item )
        removeItem( item );
}


void 
PlayerBucket::removeItem( PlayableListItem* item )
{
    setCurrentItem( NULL );
    emit itemRemoved( item->text(), (Seed::Type)item->playableType());
    delete item;
    calculateLayout();
    viewport()->update();
    return;   
}


void 
PlayerBucket::clearItems()
{
    while( model()->rowCount() )
    {
        removeIndex( model()->index( 0, 0 ));
    }
    calculateLayout();
}



void 
PlayerBucket::removeCurrentItem()
{
    if( currentIndex().isValid() )
        removeIndex( currentIndex() );
}


void 
PlayerBucket::showQuery()
{
    ui.queryEditButton->hide();
    ui.queryEdit->show();
    calculateLayout();
}


void 
PlayerBucket::onCurrentItemChanged( QListWidgetItem* current, QListWidgetItem* previous )
{
    if( current ) 
    {
        calculateLayout();
    }
    else
    {
        ui.removeButton->hide();
    }
}


void 
PlayerBucket::mousePressEvent( QMouseEvent* event )
{
    if( event->button() == Qt::LeftButton &&
        !indexAt( event->pos()).isValid())
    {
        //clear selection when mouse is clicked outside
        //of any seed items
        setCurrentItem( NULL );
        viewport()->update();
    }
    
    QListWidget::mousePressEvent( event );
}


void 
PlayerBucket::onQueryEditReturn()
{
    RadioStation station = RadioStation::rql( ui.queryEdit->text() );
    
    station.setTitle( "" );
    The::radio().play( station );
}

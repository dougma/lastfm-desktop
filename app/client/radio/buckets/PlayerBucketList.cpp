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

#include "PlayerBucketList.h"
#include "Sources.h"
#include "SeedDelegate.h"
#include "PlayableListItem.h"
#include "PlayableMimeData.h"
#include "lib/lastfm/ws/WsAccessManager.h"
#include <QVBoxLayout>
#include <QListView>
#include <QScrollBar>
#include <QPushButton>
#include "widgets/ImageButton.h"
#include "widgets/UnicornWidget.h"
#include "lib/lastfm/radio/RadioStation.h"
#include "the/radio.h"

Q_DECLARE_METATYPE( PlayableListItem* )

const int PlayerBucketList::k_itemMargin = 4;

//These should be based on the delegate's sizeHint - this requires
//the delegate to calculate the sizeHint correctly however and this 
//is not currently done!
const int PlayerBucketList::k_itemSizeX = 66;
const int PlayerBucketList::k_itemSizeY = 88;


PlayerBucketList::PlayerBucketList( QWidget* w )
			: QListWidget( w ),
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

    connect( ui.queryEditButton, SIGNAL( clicked()), SLOT( showQuery()));

    ui.queryEdit = new QLineEdit( this );
    ui.queryEdit->setAttribute( Qt::WA_MacShowFocusRect, false );
    QPalette queryPalette = ui.queryEdit->palette();
    queryPalette.setBrush( QPalette::Base, Qt::transparent );
    queryPalette.setBrush( QPalette::Text, Qt::white );
    ui.queryEdit->setPalette( queryPalette );
    ui.queryEdit->setAlignment( Qt::AlignCenter );
    ui.queryEdit->setFrame( false );
    
    QPalette viewPortPalette = viewport()->palette();
    viewPortPalette.setBrush( QPalette::Text, QBrush( 0x777777));
    viewport()->setPalette( viewPortPalette );
    viewport()->setAutoFillBackground( false );

    connect( ui.queryEdit, SIGNAL( returnPressed()), SLOT( onQueryEditReturn()));

    setAttribute( Qt::WA_MacShowFocusRect, false );

    setFrameShape( QFrame::NoFrame );
	setItemDelegate( new SeedDelegate( this ));
	setAcceptDrops( true );
    setDragDropMode( QAbstractItemView::DragDrop );
    setDropIndicatorShown( false );
    
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

	setSelectionMode( QAbstractItemView::ExtendedSelection );
	setContextMenuPolicy( Qt::CustomContextMenu );

    calculateLayout();
}


void
PlayerBucketList::paintEvent( QPaintEvent* event )
{
    Q_UNUSED( event );
    QPainter p( viewport() );
   
    QAbstractItemModel* itemModel = model();
	if( !itemModel->rowCount() )
	{
        QFont f = p.font();
        f.setBold( true );
        f.setPointSize( 12 );
        p.setFont( f );

		p.drawText( viewport()->rect(), 
		            Qt::AlignCenter | Qt::TextWordWrap, 
		            tr("Drag something in here to play it.") );
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
PlayerBucketList::scrollContentsBy( int dx, int dy )
{
    Q_UNUSED( dx ); Q_UNUSED( dy );
    calculateLayout();
    viewport()->update();
}


void 
PlayerBucketList::resizeEvent ( QResizeEvent* event )
{
	Q_UNUSED( event );
    calculateLayout();
}


void 
PlayerBucketList::calculateLayout()
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
PlayerBucketList::calculateToolIconsLayout()
{
    //show/hide clear button

    if( model()->rowCount() <= 0 )
    {
        ui.clearButton->hide();
        ui.queryEdit->hide();
        ui.queryEditButton->hide();
        return;
    }
    
    const int scrollbarOffset = verticalScrollBar()->isVisible() ? verticalScrollBar()->width() : 0;
    const int iconSpacing = 5;
    
    ui.clearButton->move( rect().translated( -ui.clearButton->width() - scrollbarOffset, 0 ).topRight());
    ui.clearButton->show();
    if( !ui.queryEdit->isVisible())
    {
        ui.queryEditButton->move( rect().translated( -ui.clearButton->width() - iconSpacing - ui.queryEditButton->width() - scrollbarOffset, 0).topRight());
        ui.queryEditButton->show();
    }
    else
    {
        ui.queryEdit->move( rect().bottomLeft() - QPoint( -8, ui.queryEdit->height()));   
        ui.queryEdit->resize( (size().width() - 16), ui.queryEdit->rect().height() );
    }

}


void 
PlayerBucketList::calculateItemRemoveLayout()
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
PlayerBucketList::dropEvent( QDropEvent* event)
{
	if( !event->mimeData() )
		return;
	
	if( addFromMimeData( event->mimeData()))
		event->acceptProposedAction();
	else
		event->ignore();
}


bool 
PlayerBucketList::addFromMimeData( const QMimeData* d )
{
	const PlayableMimeData* data = qobject_cast< const PlayableMimeData* >( d );
	if( !data )
		return false;
	
	PlayableListItem* item = PlayableListItem::createFromMimeData( data );
	item->setForeground( Qt::white );
	item->setBackground( QColor( 0x2e, 0x2e, 0x2e));
	item->setFlags( item->flags() ^ Qt::ItemIsDragEnabled );
    
    if( item->playableType() == Seed::PreDefinedType )
        item->setRQL( data->rql());
    
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
PlayerBucketList::queryString() const
{
    QString query = queryString( model()->index( 0, 0 ), false );
    for( int i = 1; i < model()->rowCount(); i++ )
    {
        QModelIndex index = model()->index( i, 0 );
        query += queryString( index );
    }
    return query;
}


/* this really needs to be moved into the seedItem delegate - I'm
   going to hold off until I've made a decision as to whether to 
   convert this to QGraphicsItems first though! - jono */
QString 
PlayerBucketList::queryString( const QModelIndex i, bool joined ) const 
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
            
        case Seed::PreDefinedType:
            if( joined )
                qs = " and ";
            qs += ((PlayableListItem*)itemFromIndex( i ))->rql();
            return qs;
	}
	qs += "\"" + i.data( Qt::DisplayRole ).toString() + "\"";
	return qs;
}


void 
PlayerBucketList::dragEnterEvent ( QDragEnterEvent * event )
{		
	event->accept();
}


QModelIndex 
PlayerBucketList::indexAt( const QPoint& point ) const
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
PlayerBucketList::visualRect ( const QModelIndex & index ) const 
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
PlayerBucketList::play()
{
    
    RadioStation station = RadioStation::rql( queryString() );
    
    station.setTitle( "" );
    The::radio().play( station );
}


bool 
PlayerBucketList::addItem( PlayableListItem* item )
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
PlayerBucketList::addItem( QListWidgetItem* item )
{
    QListWidget::addItem( item );
    calculateLayout();
}


void 
PlayerBucketList::removeIndex( const QModelIndex& index )
{
    PlayableListItem* item = static_cast<PlayableListItem*>( itemFromIndex( index ));
    if( item )
        removeItem( item );
}


void 
PlayerBucketList::removeItem( PlayableListItem* item )
{
    setCurrentItem( NULL );
    emit itemRemoved( item->text(), (Seed::Type)item->playableType());
    delete item;
    calculateLayout();
    viewport()->update();
    return;   
}


void 
PlayerBucketList::clearItems()
{
    while( model()->rowCount() )
    {
        removeIndex( model()->index( 0, 0 ));
    }
    calculateLayout();
}



void 
PlayerBucketList::removeCurrentItem()
{
    if( currentIndex().isValid() )
        removeIndex( currentIndex() );
}


void 
PlayerBucketList::showQuery()
{
    if( ui.queryEdit->isVisible() )
    {
        ui.queryEdit->hide();
        calculateLayout();
    }
    else
    {
        ui.queryEdit->show();
        calculateLayout();
    }
}


void 
PlayerBucketList::onCurrentItemChanged( QListWidgetItem* current, QListWidgetItem* previous )
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
PlayerBucketList::mousePressEvent( QMouseEvent* event )
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
PlayerBucketList::onQueryEditReturn()
{
    RadioStation station = RadioStation::rql( ui.queryEdit->text() );
    
    station.setTitle( "" );
    The::radio().play( station );
}

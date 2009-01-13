/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
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

#include "TagListWidget.h"
#include "Settings.h"
#include "lib/lastfm/types/Tag.h"
#include "lib/lastfm/ws/WsReply.h"
#include "the/mainWindow.h"
#include "radio/buckets/DelegateDragHint.h"
#include "radio/buckets/Amp.h"
#include "radio/buckets/SeedsWidget.h"
#include "PlayableMimeData.h"
#include <QDesktopServices>
#include <QHeaderView>
#include <QItemDelegate>
#include <QMenu>
#include <QUrl>


TagListWidget::TagListWidget( QWidget* parent ) 
             : QTreeWidget( parent )
			 , m_currentReply( 0 )
{
    setColumnCount( 2 );
    setRootIsDecorated( false );
    setContextMenuPolicy( Qt::CustomContextMenu );
    setFrameStyle( NoFrame );
    setAlternatingRowColors( true );
    setDragEnabled( true );

    class TallerRowDelegate : public QItemDelegate
    {
        virtual QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
        {
            return QItemDelegate::sizeHint( option, index ) + QSize( 0, 4 );
        }
    };

    setItemDelegate( new TallerRowDelegate );
    
    QTreeWidget::hideColumn( 1 );
    QTreeWidget::header()->hide();
    
    m_menu = new QMenu( this );
    QActionGroup* group = new QActionGroup( this );
    
    QAction* a = m_menu->addAction( tr( "Sort by Popularity" ) );
    connect( a, SIGNAL(triggered()), SLOT(sortByPopularity()) );
    group->addAction( a );
    a->setCheckable( true );
    a->setChecked( true );

    a = m_menu->addAction( tr( "Sort Alphabetically" ) );
    connect( a, SIGNAL(triggered()), SLOT(sortAlphabetically()) );
    group->addAction( a );
    a->setCheckable( true );
    
    m_menu->addSeparator();
    a = m_menu->addAction( tr("Open Last.fm Page for this Tag") );
    connect( a, SIGNAL(triggered()), SLOT(openTagPageForCurrentItem()) );
    
    connect( this, SIGNAL(customContextMenuRequested( QPoint )), SLOT(showMenu( QPoint )) );
    connect( this, SIGNAL(doubleClicked( const QModelIndex& )), SLOT(onDoubleClicked ( const QModelIndex& )) );
}


QTreeWidgetItem*
TagListWidget::createNewItem( QString tag )
{
    tag = tag.toLower();
    
    QTreeWidgetItem* item = new QTreeWidgetItem( QStringList() << tag );
    QIcon icon;
    icon.addPixmap( QPixmap( ":/buckets/tag.png" ) );
    item->setIcon( 0, icon );

    addTopLevelItem( item );

    return item;
}


bool
TagListWidget::add( QString tag )
{
    //FIXME avoid duplicates
    createNewItem( tag );
    m_newTags += tag;
    return true;
}


void
TagListWidget::showMenu( const QPoint& point )
{
    m_menu->exec( mapToGlobal( point ) );
}


void
TagListWidget::sortAlphabetically()
{
    sortItems( 0, Qt::AscendingOrder );
}


void
TagListWidget::sortByPopularity()
{
    //I got here and wasn't sure if sortItems() should be used instead either --mxcl
    sortByColumn( 1, Qt::DescendingOrder );
}


void
TagListWidget::openTagPageForCurrentItem()
{
	QDesktopServices::openUrl( Tag( currentItem()->text( 0 ) ).www() );
}


void
TagListWidget::setTagsRequest( WsReply* r )
{
    clear();
	delete m_currentReply;
	m_currentReply = r;
    connect( (QObject*)r, SIGNAL(finished( WsReply* )), SLOT(onTagsRequestFinished( WsReply* )) );
}


void
TagListWidget::onTagsRequestFinished( WsReply* r )
{    
	foreach (WeightedString tag, Tag::list( r ))
	{
        QTreeWidgetItem *entry = createNewItem( tag );
		// I couldn't make it sort properly otherwise, even the QVariant methods wouldn't work!
		entry->setText( 1, QString::number( 10 * 1000 + tag.weighting() ) );
	}
	m_currentReply = 0;
}


QMimeData* 
TagListWidget::mimeData( const QList<QTreeWidgetItem *> items ) const
{
    if( items.count() < 1 )
        return 0;
    
    Tag tag( items.first()->text( 0 ) );
    PlayableMimeData* pData = PlayableMimeData::createFromTag( tag );
    
    return pData;
}


#include <QPainter>
TagIconView::TagIconView()
{
    setAlternatingRowColors( false );
    disconnect( this, SIGNAL(customContextMenuRequested( QPoint )), 0, 0 );
}


void
TagIconView::paintEvent( QPaintEvent* e )
{    
    TagListWidget::paintEvent( e );

    if (topLevelItemCount()) 
        return;
    
    QPainter p( viewport() );
    p.setPen( Qt::lightGray );
#ifndef WIN32
    QFont f = p.font();
    f.setPointSize( 15 );
    p.setFont( f );
#endif
    p.drawText( viewport()->rect(), Qt::AlignCenter, tr("Type a tag above,\nor choose from below") );
}

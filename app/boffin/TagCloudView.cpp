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
 
#include "TagCloudView.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>

static const int k_RightMargin = 10;


TagCloudView::TagCloudView( QWidget* parent ) 
             : QAbstractItemView( parent )
{
    QFont f = font();

#ifdef Q_WS_WIN
    f.setPointSize( 10 );
#else
    f.setPointSize( 14 );
#endif

    setFont( f );
    viewport()->setMouseTracking( true );
    
    setSelectionMode( QAbstractItemView::MultiSelection );
    setSelectionBehavior( QAbstractItemView::SelectItems );

    //Needed to repaint on mouse move:
    viewport()->setAttribute( Qt::WA_Hover );
}


TagCloudView::~TagCloudView()
{
    m_rectIndex.clear();
}


void 
TagCloudView::setSelection( const QRect& rect, QItemSelectionModel::SelectionFlags f )
{
    if (state() == DragSelectingState)
        return;

    QRect r = rect.translated( 0, verticalScrollBar()->value());
    foreach( QModelIndex i, m_rectIndex.keys() )
    {
        if( m_rectIndex[ i ].intersects( r ))
        {
            selectionModel()->select(i, f);
        }
    }
}


QStringList 
TagCloudView::currentTags() const
{
    QStringList ret;
    foreach( QModelIndex i, selectionModel()->selectedIndexes() )
        ret << i.data( Qt::DisplayRole ).toString();
    
    return ret;
}


void 
TagCloudView::paintEvent( QPaintEvent* e )
{    
    QPainter p( viewport() );
    p.setClipRect( e->rect());
    QStyleOptionViewItem opt = viewOptions();
    
    QHash< QModelIndex, QRect >::const_iterator i = m_rectIndex.constBegin();

    if( model()->rowCount() == 0 )
    {
        p.drawText( viewport()->rect(), Qt::AlignCenter,  "No tags have been found!" );
        
        return;
    }

    for( ; i != m_rectIndex.constEnd(); ++i )
    {
        const QModelIndex& index = i.key();
        const QRect& rect = i.value();

        opt.state = (index != m_hoverIndex || !isEnabled() ? QStyle::State_None : QStyle::State_MouseOver);

        opt.rect = rect.translated( 0, -verticalScrollBar()->value());
        
        if( selectionModel()->isSelected( index ) )
            opt.state |= QStyle::State_Selected;

        itemDelegate()->paint( &p, opt, index );
    }
}

#include <limits.h>
void 
TagCloudView::updateGeometries()
{
    int rowHeight = 0;
    QStyleOptionViewItem opt = viewOptions();
    int minRowHeight = INT_MAX;
    for( int i = 0; i < model()->rowCount(); ++i )
    {
        QModelIndex index = model()->index( i, 0 );

        if( rowHeight == 0 )
        {
            QRect rect = viewport()->rect();
            rect.setSize( itemDelegate()->sizeHint( opt, index ));
            int count = i;
            int x = rect.right();
            while( x < viewport()->rect().right() && count < model()->rowCount() - 1)
            {
                const QSize sizeHint = itemDelegate()->sizeHint( opt, model()->index( count, 0 )); 
                rowHeight = qMax( rowHeight, sizeHint.height());
                x += itemDelegate()->sizeHint( opt, model()->index( count + 1, 0 )).width() + k_RightMargin;
                count++;
            }
            minRowHeight = rowHeight > 0 && rowHeight < minRowHeight ? rowHeight : minRowHeight;
        }

        opt.state = (index != m_hoverIndex ? QStyle::State_None : QStyle::State_MouseOver);

        if( selectionModel()->isSelected( index ) )
            opt.state |= QStyle::State_Selected;

        opt.rect.setSize( itemDelegate()->sizeHint( opt, index ));
        opt.rect.translate( 0, ( rowHeight - opt.rect.height()));

        m_rectIndex.insert( index, opt.rect );
        
        opt.rect.translate( opt.rect.width() + k_RightMargin, -( rowHeight - opt.rect.height()) );
        
        if( i < model()->rowCount() -1 &&  opt.rect.right() + itemDelegate()->sizeHint( opt, model()->index( i + 1, 0 )).width() > viewport()->rect().right() + k_RightMargin )
        {
            opt.rect.moveLeft( viewport()->rect().left());
            opt.rect.moveTop( opt.rect.top() + rowHeight ); 
            rowHeight = 0;
        }
    }

    verticalScrollBar()->setRange( 0, opt.rect.bottom() -viewport()->rect().height() );
    verticalScrollBar()->setPageStep( viewport()->height() );
    verticalScrollBar()->setSingleStep( minRowHeight ); 


    QAbstractItemView::updateGeometries();
}


void 
TagCloudView::selectAll()
{
    QItemSelection allSelection( model()->index( 0 , 0 ), model()->index( model()->rowCount(), 0 ));
    selectionModel()->select( allSelection, QItemSelectionModel::Toggle );
    viewport()->update();
}


QModelIndex 
TagCloudView::indexAt( const QPoint& pos ) const
{
    QPoint p = pos + QPoint( 0, verticalScrollBar()->value());
    foreach( QModelIndex i, m_rectIndex.keys() )
    {
        if( m_rectIndex[ i ].contains( p ))
        {
            return i;
        }
    }
    return QModelIndex();
}


QRect 
TagCloudView::visualRect( const QModelIndex& i ) const
{
    return m_rectIndex[ i ].translated( 0, -verticalScrollBar()->value());
}


bool
TagCloudView::viewportEvent( QEvent* event )
{
    switch( event->type() )
    {
        case QEvent::MouseMove:
        {
            QMouseEvent* e = static_cast< QMouseEvent* >( event );
            m_hoverIndex = indexAt( e->pos() );
        }
        break;
        default:
        break;
    }
    return QAbstractItemView::viewportEvent( event );
}


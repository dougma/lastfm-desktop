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
#include <QPainter>
#include <QMouseEvent>

static const int k_RightMargin = 10;

#include <QDebug>

TagCloudView::TagCloudView( QWidget* parent ) 
             : QAbstractItemView( parent )
{
    QFont f = font();
    f.setPointSize( 14 );
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
TagCloudView::setSelection( const QRect& r, QItemSelectionModel::SelectionFlags f )
{
    if (state() == DragSelectingState)
        return;
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
    int rowHeight = 0;

    QHash< QModelIndex, QRect >::const_iterator i = m_rectIndex.constBegin();
    while( i != m_rectIndex.constEnd() )
    {
        const QModelIndex& index = i.key();
        const QRect& rect = i.value();

        opt.state = (index != m_hoverIndex ? QStyle::State_None : QStyle::State_MouseOver);

        opt.rect = rect;
        
        if( selectionModel()->isSelected( index ) )
            opt.state |= QStyle::State_Selected;

        itemDelegate()->paint( &p, opt, index );

        i++;
    }
}


void 
TagCloudView::updateGeometries()
{
    int rowHeight = 0;
    QStyleOptionViewItem opt = viewOptions();
    for( int i = 0; i < model()->rowCount(); i++ )
    {
        QModelIndex index = model()->index( i, 0 );

        if( rowHeight == 0 )
        {
            QRect rect = viewport()->rect();
            rect.setSize( itemDelegate()->sizeHint( opt, index ));
            int count = i;
            int x = rect.right();
            while( rect.right() < (viewport()->rect().right() - k_RightMargin) && count < model()->rowCount())
            {
                const QSize sizeHint = itemDelegate()->sizeHint( opt, model()->index( count, 0 )); 
                x += sizeHint.width() + k_RightMargin;
                rowHeight = qMax( rowHeight, sizeHint.height());
                count++;
            }
        }


        opt.state = (index != m_hoverIndex ? QStyle::State_None : QStyle::State_MouseOver);

        if( selectionModel()->isSelected( index ) )
            opt.state |= QStyle::State_Selected;

        opt.rect.setSize( itemDelegate()->sizeHint( opt, index ));
        opt.rect.translate( 0, ( rowHeight - (opt.rect.height() * 0.9)));

        m_rectIndex.insert( index, opt.rect );
        
        opt.rect.translate( opt.rect.width() + k_RightMargin, -( rowHeight - (opt.rect.height() * 0.9)) );
        
        if( opt.rect.right() + k_RightMargin > viewport()->rect().right())
        {
            opt.rect.moveLeft( rect().left());
            opt.rect.moveTop( opt.rect.top() + rowHeight ); 
            rowHeight = 0;
        }
    }
    
    QAbstractItemView::updateGeometries();
}


QModelIndex 
TagCloudView::indexAt( const QPoint& p ) const
{
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
    return m_rectIndex[ i ];
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


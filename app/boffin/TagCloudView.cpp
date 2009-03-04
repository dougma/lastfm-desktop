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
#include "TagCloudModel.h"
#include <QApplication>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <limits.h>


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
}


void 
TagCloudView::setSelection( const QRect& rect, QItemSelectionModel::SelectionFlags f )
{
    if (state() == DragSelectingState)
        return;

    QRect r = rect.translated( 0, verticalScrollBar()->value());
    foreach( QModelIndex i, m_rects.keys() )
    {
        if( m_rects[ i ].intersects( r ))
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
    
    QHash< QModelIndex, QRect >::const_iterator i = m_rects.constBegin();

    if( model()->rowCount() == 0 )
    {
        p.drawText( viewport()->rect(), Qt::AlignCenter,  "No tags have been found!" );
        return;
    }

    for( ; i != m_rects.constEnd(); ++i )
    {
        const QModelIndex& index = i.key();
        const QRect& rect = i.value();

        opt.state = QStyle::State_None;
        if( m_hoverIndex == index && isEnabled() )
            opt.state = qApp->mouseButtons() == Qt::NoButton
                    ? QStyle::State_MouseOver
                    : QStyle::State_Active;

        opt.rect = rect.translated( 0, -verticalScrollBar()->value() );
        
        if( selectionModel()->isSelected( index ) )
            opt.state |= QStyle::State_Selected;

        if( isEnabled() )
            opt.state |= QStyle::State_Enabled;

        itemDelegate()->paint( &p, opt, index );
    }
}

int gBaseline, gLeftMargin; //filthy but easiest
void
TagCloudView::rectcalc()
{
    QStyleOptionViewItem const opt = viewOptions();
    int baseline = 0;

    for (int j = 0; j < model()->rowCount(); ++j)
    {
        QModelIndex const i = model()->index( j, 0 );
        QRect r( QPoint(), itemDelegate()->sizeHint( opt, i ) );
        if (baseline == 0)
            baseline = gBaseline;
            
        r.moveTo( gLeftMargin, baseline-gBaseline );
            
        m_rects[i] = r;
    }
}


void
TagCloudView::updateGeometries()
{
    rectcalc(); //TODO only needs to be done once when data is set!
    
    const int VIEWPORT_MARGIN = 10;
    
    int y = VIEWPORT_MARGIN;
    int left_margin = 0; // the left baseline to align text against
    
    for (int j = 0; j < model()->rowCount(); ++j)
    {
        QModelIndex const i = model()->index( j, 0 );
        QRect r = m_rects[i];
        
        if (left_margin == 0) left_margin = r.x();
    
        // do new row
        int x = VIEWPORT_MARGIN + (left_margin - r.x());
        int tallest = 0;
        for (; j < model()->rowCount(); ++j)
        {
            QModelIndex const i = model()->index( j, 0 );
            QRect r = m_rects[i];
            
            r.moveTo( x, y + r.y() );

            x += r.width();
            if (tallest != 0 //need at least one thing per row
                && x > viewport()->width() - VIEWPORT_MARGIN) { --j; break; }

            m_rects[i] = r;

            tallest = qMax( tallest, r.bottom() - y );
        }
        
        y += tallest;
    }
    
    verticalScrollBar()->setRange( 0, y + VIEWPORT_MARGIN - viewport()->height() );
    verticalScrollBar()->setPageStep( viewport()->height() );
    verticalScrollBar()->setSingleStep( 20 /*TODO*/ ); 

    QAbstractItemView::updateGeometries();
}


void 
TagCloudView::selectAll()
{
    QItemSelection allSelection( model()->index( 0 , 0 ), model()->index( model()->rowCount()-1, 0 ));
    selectionModel()->select( allSelection, QItemSelectionModel::Toggle );
    viewport()->update();
}


QModelIndex 
TagCloudView::indexAt( const QPoint& pos ) const
{
    QPoint p = pos + QPoint( 0, verticalScrollBar()->value());
    foreach( QModelIndex i, m_rects.keys() )
    {
        if( m_rects[ i ].contains( p ))
        {
            return i;
        }
    }
    return QModelIndex();
}


QRect 
TagCloudView::visualRect( const QModelIndex& i ) const
{
    return m_rects[ i ].translated( 0, -verticalScrollBar()->value());
}


bool
TagCloudView::viewportEvent( QEvent* event )
{
    switch( event->type() )
    {
        case QEvent::Wheel:
            {
                // do first so it moves the view
                bool b = QAbstractItemView::viewportEvent( event );
                // then calculate the new tag under the mouse
                m_hoverIndex = indexAt( viewport()->mapFromGlobal( QCursor::pos() ) );
                return b;
            }
            
        case QEvent::MouseMove:
        {
            QMouseEvent* e = static_cast< QMouseEvent* >( event );
            QModelIndex const oldindex = m_hoverIndex;
            m_hoverIndex = indexAt( e->pos() );
            if (oldindex != m_hoverIndex)
                viewport()->update();
            break;
        }
        
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
            if (m_hoverIndex.isValid())
                viewport()->update();
            break;

        case QEvent::KeyPress:
        case QEvent::KeyRelease:
            return false;

        default:
            break;
    }
    return QAbstractItemView::viewportEvent( event );
}




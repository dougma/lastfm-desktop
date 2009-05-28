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
#include "PlaydarTagCloudModel.h"
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <limits.h>


TagCloudView::TagCloudView( QWidget* parent )
             : QAbstractItemView( parent )
             , m_fetched( false )
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
TagCloudView::onTag( const BoffinTagItem& tag )
{
	if( m_fetched )
		return;

	m_loadedTag = tag.m_name;
	viewport()->update();
}

void
TagCloudView::setSelection( const QRect& rect, QItemSelectionModel::SelectionFlags f )
{
    if (state() == DragSelectingState)
        return;

    QRect r = rect.translated( 0, verticalScrollBar()->value());
    RectsConstIt it = m_rects.constBegin();
    for (int c = 0; it != m_rects.constEnd(); it++, c++) {
        if (it->intersects(r)) {
            selectionModel()->select(model()->index(c, 0), f);
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

    if (!m_fetched) {
		p.drawText( viewport()->rect(), Qt::AlignCenter, "Fetching tags.." + m_loadedTag );
		return;
    }

    if (m_rects.isEmpty()) {
        p.drawText( viewport()->rect(), Qt::AlignCenter,  "No tags have been found!" );
        return;
    }

    QStyleOptionViewItem opt = viewOptions();
    RectsConstIt it = m_rects.constBegin();
    for (int c = 0; it != m_rects.constEnd(); it++, c++) 
    {
        opt.rect = it->translated( 0, -verticalScrollBar()->value() );

        if( e->rect().intersects( opt.rect )) {
            const QModelIndex& index = model()->index(c, 0);

            opt.state = QStyle::State_None;
            if( m_hoverIndex == index && isEnabled() )
                opt.state = qApp->mouseButtons() == Qt::NoButton
                        ? QStyle::State_MouseOver
                        : QStyle::State_Active;

            if( selectionModel()->isSelected( index ) )
                opt.state |= QStyle::State_Selected;

            if( isEnabled() )
                opt.state |= QStyle::State_Enabled;

            itemDelegate()->paint( &p, opt, index );
        }
    }
}


void
TagCloudView::onFetchedTags()
{
	m_fetched = true;
}

void
TagCloudView::setModel(QAbstractItemModel *model)
{
    QAbstractItemView::setModel(model);
    connect(model, SIGNAL(rowsInserted(QModelIndex, int, int)), SLOT(onRowsInserted(QModelIndex, int, int)));
    connect(model, SIGNAL(rowsRemoved(QModelIndex, int, int)), SLOT(onRowsRemoved(QModelIndex, int, int)));
//    connect(model, SIGNAL(modelReset()), SLOT(onModelReset()));
}

void
TagCloudView::onRowsInserted(const QModelIndex & parent, int start, int end)
{
	Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
//    updateGeometries();
}

void
TagCloudView::onRowsRemoved(const QModelIndex & parent, int start, int end)
{
	Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
//    updateGeometries();
}


int gBaseline, gLeftMargin; //filthy but easiest
void
TagCloudView::rectcalc()
{
    QStyleOptionViewItem const opt = viewOptions();
    int baseline = 0;

    m_rects.clear();
    for (int j = 0; j < model()->rowCount(); ++j)
    {
        QModelIndex const i = model()->index( j, 0 );
        QRect r( QPoint(), itemDelegate()->sizeHint( opt, i ) );
        if (baseline == 0)
            baseline = gBaseline;

        r.moveTo( gLeftMargin, baseline-gBaseline );

        m_rects.insert(j, r);
    }
}


void
TagCloudView::updateGeometries()
{
    if (!model())
        return;

    rectcalc(); //TODO only needs to be done once when data is set!

    const int VIEWPORT_MARGIN = 10;

    int y = VIEWPORT_MARGIN;
    int left_margin = 0; // the left baseline to align text against

    // iterate in model-order to lay the tags out left to right
    for (int j = 0; j < model()->rowCount(); ++j)
    {
        QRect r = m_rects[j];
        if (left_margin == 0) 
            left_margin = r.x();

        // do new row
        int x = VIEWPORT_MARGIN + (left_margin - r.x());
        int tallest = 0;
        for (; j < model()->rowCount(); ++j)
        {
            QRect r = m_rects[j];
            r.moveTo( x, y + r.y() );

            x += r.width();
            if (tallest != 0 //need at least one thing per row
                && x > viewport()->width() - VIEWPORT_MARGIN) 
            { 
                --j; break; 
            }

            m_rects[j] = r;
            tallest = qMax( tallest, r.bottom() - y );
        }

        y += tallest;
    }

    verticalScrollBar()->setRange( 0, y + VIEWPORT_MARGIN - viewport()->height() );
    verticalScrollBar()->setPageStep( viewport()->height() );
    verticalScrollBar()->setSingleStep( 20 /*TODO*/ );

    viewport()->update();
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
    RectsConstIt it = m_rects.constBegin();
    for (int c = 0; it != m_rects.constEnd(); it++, c++) {
        if( it->contains( p )) {
            return model()->index(c, 0);
        }
    }
    return QModelIndex();
}


QRect
TagCloudView::visualRect( const QModelIndex& i ) const
{
    return m_rects[ i.row() ].translated( 0, -verticalScrollBar()->value());
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




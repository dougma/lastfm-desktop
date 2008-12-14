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

#include "FirehoseView.h"
#include <QtGui>


FirehoseView::FirehoseView() : h( 0 ), offset( 0 )
{            
    timer = new QTimeLine( 1200, this );
	timer->setUpdateInterval( 10 );
    connect( timer, SIGNAL(frameChanged( int )), SLOT(onFrameChange( int )) );
    
    setAttribute( Qt::WA_MacShowFocusRect, false );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    bar()->setRange( 0, 0 );
}


void
FirehoseView::setModel( QAbstractItemModel* p )
{
    model = p;
    model->setParent( this );
    connect( model, SIGNAL(rowsInserted( QModelIndex, int, int )), SLOT(onRowInserted()) );
    connect( model, SIGNAL(modelReset()), SLOT(onModelReset()) );
    connect( timer, SIGNAL(finished()), model, SLOT(prune()) );
}


void
FirehoseView::setDelegate( QAbstractItemDelegate* p )
{
    delegate = p;
    delegate->setParent( this );
    
    QStyleOptionViewItem opt;
    opt.font = font();
    h = delegate->sizeHint( opt, QModelIndex() ).height();
    timer->setFrameRange( -h, 0 );
    bar()->setSingleStep( h / 3 );
}


void
FirehoseView::scrollContentsBy( int, int dy )
{
    QAbstractScrollArea::scrollContentsBy( 0, dy );
}


void
FirehoseView::onRowInserted()
{
    //TODO handle row inserting while we're animating
    offset = 0;
    timer->start();
}


// don't let the scrollbar get larger than 20, or less than 0
#define SET_MAXIMUM bar()->setMaximum( qBound( 0, model->rowCount()*h + offset -bar()->pageStep(), h*20 - bar()->pageStep() ) )
void
FirehoseView::onFrameChange( int i )
{
    offset = i;
    SET_MAXIMUM;
    viewport()->update();
}


void
FirehoseView::onModelReset()
{
    bar()->setRange( 0, 0 );
    viewport()->update();
}


void
FirehoseView::paintEvent( QPaintEvent* )
{
    QPainter p( viewport() );
    
    QStyleOptionViewItem opt;
    opt.font = font();
    opt.palette = palette();
    opt.state = QStyle::State_Active;
    opt.rect = QRect( 0, 0, viewport()->width(), h );
    
    int const dy = bar()->value();
    
    for (int x = 0, n = model->rowCount(); x < n; ++x)
    {
        opt.rect.moveTop( x*h + offset - dy );
        delegate->paint( &p, opt, model->index( x, 0 ) );
    }
}


void
FirehoseView::resizeEvent( QResizeEvent* )
{
    bar()->setPageStep( viewport()->height() );
    SET_MAXIMUM;
}

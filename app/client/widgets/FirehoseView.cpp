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
#include "FirehoseDelegate.h"
#include <QtGui>
#include "the/MainWindow.h"
#include "radio/Buckets/PrimaryBucket.h"

FirehoseView::FirehoseView() : h( 0 ), offset( 0 )
{            
    timer = new QTimeLine( 1200, this );
    connect( timer, SIGNAL(frameChanged( int )), SLOT(onFrameChange( int )) );
    
    setAttribute( Qt::WA_MacShowFocusRect, false );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
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
    //TODO cliprect or not paint stuff not visible?
    
    QPainter p( viewport() );
    
    if (model->rowCount() == 0)
    {    
        QFont f = p.font();
        f.setBold( true );
        f.setPixelSize( 16 ); // indeed pixels are fine on mac and windows, not linux though
        p.setFont( f );
        p.drawText( rect().adjusted( 5, 5, -5, -5 ),
                    Qt::AlignCenter, 
                    QString::fromUtf8("Empty View Text™") );
    }
    
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
        m_itemRects[ model->index( x, 0 )] = opt.rect;
    }
}


void
FirehoseView::resizeEvent( QResizeEvent* )
{
    bar()->setPageStep( viewport()->height() );
    SET_MAXIMUM;
}


QModelIndex 
FirehoseView::indexAt( const QPoint& point ) const
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


void 
FirehoseView::mouseDoubleClickEvent( QMouseEvent* e )
{
    QModelIndex i = indexAt( e->pos() );
    
    PlayableMimeData* data = PlayableMimeData::createFromUser( i.data().toString() );
    data->setImageData( i.data( Qt::DecorationRole ) );
    
    //FIXME: This is soo incredibly unencapsulated! (applies to SimilarArtists, TagListWidget and FirehoseView )
    The::mainWindow().ui.primaryBucket->replaceStation( data );
}


void 
FirehoseView::mousePressEvent( QMouseEvent* event )
{
    if( event->button() == Qt::LeftButton )
        m_dragStartPosition = event->pos();
}


void
FirehoseView::mouseMoveEvent( QMouseEvent* event )
{
    if( !(event->buttons() & Qt::LeftButton) )
        return;
    
    if( (event->pos() - m_dragStartPosition).manhattanLength()
        < QApplication::startDragDistance() )
        return;
       
    QDrag *drag = new QDrag( this );
    QModelIndex i = indexAt( event->pos() );
    PlayableMimeData* mimeData = PlayableMimeData::createFromUser( i.data().toString() );
    mimeData->setImageData( i.data( Qt::DecorationRole ).value<QImage>() );
    
    drag->setMimeData(mimeData);
    
    QPixmap pixmap;
    {
        QImage image( m_itemRects[ i ].size(), QImage::Format_ARGB32_Premultiplied );
        QPainter painter( &image );
        QStyleOptionViewItem opt;
        opt.font = font();
        opt.palette = palette();
        opt.state = QStyle::State_Active;
        opt.rect = m_itemRects[ i ];
        opt.rect.moveTo( 0, 0 );
        delegate->paint( &painter, opt, i );
        pixmap = QPixmap::fromImage( image );
    }
    drag->setPixmap( pixmap );
    qDebug() << "Mouse press position: " << event->pos();
    qDebug() << "Hotspot position: " << event->pos() - QPoint( 0, m_itemRects[ i ].top());
    drag->setHotSpot( event->pos() - QPoint( 0, m_itemRects[ i ].top()) );
    
    Qt::DropAction dropAction = drag->exec();
    
}
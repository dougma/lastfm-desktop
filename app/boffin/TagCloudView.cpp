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

TagCloudView::TagCloudView( QWidget* parent ) 
             : QAbstractItemView( parent )
{
    QFont f = font();
    f.setPointSize( 50 );
    setFont( f );
    viewport()->setMouseTracking( true );
    
    //Needed to repaint on mouse move:
    viewport()->setAttribute( Qt::WA_Hover );
}


TagCloudView::~TagCloudView()
{
    m_rectIndex.clear();
}


void 
TagCloudView::paintEvent( QPaintEvent* )
{    
    QPainter p( viewport() );
    QStyleOptionViewItem opt;
    opt.initFrom( this );
    opt.font = font();
    for( int i = 0; i < model()->rowCount(); i++ )
    {
        QModelIndex index = model()->index( i, 0 );

        opt.state = index != m_hoverIndex ? QStyle::State_None : QStyle::State_MouseOver;

        opt.rect.setSize( itemDelegate()->sizeHint( opt, index ));
        if( opt.rect.right() > rect().right())
        {
            opt.rect.moveLeft( rect().left());
            opt.rect.moveTop( opt.rect.bottom()); 
        }
        m_rectIndex.insert( index, opt.rect );
        itemDelegate()->paint( &p, opt, index );
        opt.rect.translate( opt.rect.width() + k_RightMargin, 0 );
    }
}


QModelIndex 
TagCloudView::indexAt( const QPoint& p ) const
{
    foreach( QModelIndex i, m_rectIndex.keys() )
    {
        if( m_rectIndex[ i ].contains( p ))
            return i;
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
            //viewport()->update();
        }
        break;
        default:
        break;
    }
    return QAbstractItemView::viewportEvent( event );
}


QString
TagCloudView::currentTag() const
{
    return "rock";
}

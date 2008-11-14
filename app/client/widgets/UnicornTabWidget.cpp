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

#include "UnicornTabWidget.h"
#include <QMouseEvent>
#include <QPainter>
#include <QTabBar>
#include <QVBoxLayout>


Unicorn::TabBar::TabBar()
{
    QFont f = font();
    f.setPointSize( 10 );
    f.setBold( false );
    setFont( f );
    
    QPalette p = palette();
    p.setBrush( QPalette::Window, QBrush( 0x111112 ) );
    p.setBrush( QPalette::Button, QBrush( 0x2d2d2e ));
    p.setBrush( QPalette::Inactive, QPalette::Button, QBrush( 0x1f1f20 ));
    p.setColor( QPalette::Active, QPalette::Text, Qt::white );
    p.setColor( QPalette::Inactive, QPalette::Text, Qt::white );
    setPalette( p );
    
    setAutoFillBackground( true );
}


QSize
Unicorn::TabBar::sizeHint() const
{
    return QSize( QTabBar::sizeHint().width(), m_active.height() - 2 );
}


void
Unicorn::TabBar::mousePressEvent( QMouseEvent* e )
{
    if (e->button() != Qt::LeftButton) {
        e->ignore();
        return;
    }
    
    int w = minimumWidth() / count();
    int hOffset = (width() - minimumWidth()) / 2.0f;   
    int index = ( (e->pos().x() - hOffset) / (w + m_spacing ) );
    
    //ignore if the click was in the spacing between tabs
    if( e->pos().x() > ( hOffset + ((index + 1) *  (w + m_spacing)) ) )
        return;
    
    setCurrentIndex( index );
}


void
Unicorn::TabBar::tabInserted( int )
{
    int w = 0;
    for (int i = 0; i < count(); ++i)
        w = qMax( fontMetrics().width( tabText( i ) ), w );
    setMinimumWidth( (w+10) * count() );
}


void
Unicorn::TabBar::tabRemoved( int i )
{
    tabInserted( i );
}


void
Unicorn::TabBar::paintEvent( QPaintEvent* e )
{
    QPainter p( this );
    
    p.setClipRect( e->rect());

    int hOffset = (width() - minimumWidth()) / 2.0f;
    int w = minimumWidth() / count();
   
    for (int i = 0; i < count(); ++i)
    {
        int const x = hOffset + (i * ( w + m_spacing ));
        
        if (i == count() - 1)
            w += (m_active.isNull() ? minimumWidth() : width()) % w;
        
        if (currentIndex() == i)
        {
            if( !m_active.isNull())
                p.fillRect( x, 0, w, height(), m_active );
            else
                p.fillRect( x, 0, w, height(), palette().brush( QPalette::Button ));
            
            p.setPen( palette().color( QPalette::Active, QPalette::Text ) );
        }
        else
        {
            if( !m_inactive.isNull())
                p.fillRect( x, 0, w, height(), m_inactive );
            else
                p.fillRect( x, 0, w, height(), palette().brush( QPalette::Inactive, QPalette::Button ));
            p.setPen( palette().color( QPalette::Inactive, QPalette::Text ) );
        }
                
        p.drawText( x, 0, w, height(), Qt::AlignCenter, tabText( i ) );
    }
    
    const int h = height() - 1;
    p.setPen( 0x29292a );
    
    p.drawLine( 0, h, width(), h );   
}


void 
Unicorn::TabBar::setSpacing( int spacing )
{
    m_spacing = spacing;
}


Unicorn::TabWidget::TabWidget()
{
    QVBoxLayout* v = new QVBoxLayout( this );
    v->addWidget( m_bar = new TabBar );
    m_bar->setSpacing( 3 );
    v->addWidget( m_stack = new QStackedWidget );
    v->setSpacing( 0 );
    v->setMargin( 0 );
    
    setAutoFillBackground( true );
    
    QPalette p = palette();
    p.setBrush( QPalette::Window, QBrush( 0x111112 ) );
    setPalette( p );
    
    setContentsMargins( 5, 5, 5, 5 );
    
    connect( m_bar, SIGNAL(currentChanged( int )), m_stack, SLOT(setCurrentIndex( int )) );
}


void
Unicorn::TabWidget::addTab( const QString& title, QWidget* w )
{
    m_bar->addTab( title );
    m_stack->addWidget( w );
    w->setAttribute( Qt::WA_MacShowFocusRect, false );
}


void 
Unicorn::TabWidget::setTabEnabled( int index, bool b )
{
    m_bar->setTabEnabled( index, b );
}

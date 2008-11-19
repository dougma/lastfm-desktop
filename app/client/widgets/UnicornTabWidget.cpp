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
#include <QHBoxLayout>

Unicorn::TabBar::TabBar()
        :m_active( ":/DockWindow/tab/active.png" )
{
    QFont f = font();
    f.setPointSize( 10 );
    f.setBold( true );
    setFont( f );
    
    QPalette p = palette();
    
    QLinearGradient window( 0, 0, 0, sizeHint().height());
    window.setColorAt( 0, 0x3c3939 );
    window.setColorAt( 1, 0x282727 );
    p.setBrush( QPalette::Window, window );
    
    QPixmap pm = m_active.copy( (m_active.width() / 2)-1, 0, 2, m_active.height());
    p.setBrush( QPalette::Button, pm );
    
    QLinearGradient buttonHighlight( 0, 0, 0, sizeHint().height() - 14 );
    buttonHighlight.setColorAt( 0, Qt::black );
    buttonHighlight.setColorAt( 1, 0x474243 );
    p.setBrush( QPalette::Midlight, buttonHighlight );
    
    p.setColor( QPalette::Active, QPalette::Text, 0x848383 );
    p.setColor( QPalette::Inactive, QPalette::Text, 0x848383 );
    setPalette( p );
    
    setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
    setFixedHeight( 33 );
    setMinimumHeight( 33 );
    
    new QHBoxLayout( this );
    layout()->setContentsMargins( 0, 0, 5, 0 );
    ((QHBoxLayout*)layout())->addStretch( 1 );
   
    setAutoFillBackground( true );
}


QSize
Unicorn::TabBar::sizeHint() const
{
    return QSize( minimumWidth(), 33 );
}


void
Unicorn::TabBar::mousePressEvent( QMouseEvent* e )
{
    if (e->button() != Qt::LeftButton) {
        e->ignore();
        return;
    }
    
    int leftMargin = 5;
    int w = minimumWidth() / count();
    int index = ( qreal(e->pos().x() - leftMargin) / (w + m_spacing ));

    if( index < count() )
        setCurrentIndex( index );
}


void
Unicorn::TabBar::tabInserted( int )
{
    int w = 0;
    for (int i = 0; i < count(); ++i)
        w = qMax( fontMetrics().width( tabText( i ) ), w );
    setMinimumWidth( (w+10) * count() + layout()->minimumSize().width());
}


void
Unicorn::TabBar::tabRemoved( int i )
{
    tabInserted( i );
}


void 
Unicorn::TabBar::addWidget( QWidget* w )
{
    layout()->addWidget( w );   
}


void
Unicorn::TabBar::paintEvent( QPaintEvent* e )
{
    QPainter p( this );
    
    p.setClipRect( e->rect());

    int w = minimumWidth() / count();
    int leftMargin = 5;
    for (int i = 0; i < count(); ++i)
    {
        int const x = leftMargin + (i * ( w + m_spacing ));
        
        if (i == count() - 1)
            w += minimumWidth() % w;
        
        if (currentIndex() == i)
        {
            p.setBrush( palette().brush( QPalette::Button ) );
            p.drawPixmap( x, 7, 8, m_active.height(), m_active, 0, 0, 8, m_active.height() );
            p.drawPixmap( x + 8, 7, w -16, m_active.height(), palette().brush( QPalette::Button ).texture());
            p.drawPixmap( x + w - 8, 7, 9, m_active.height(), m_active, m_active.width() / 2, 0, 9, m_active.height() );
            p.setPen( palette().color( QPalette::Active, QPalette::Text ) );
        }
        else
        {
            p.setPen( palette().color( QPalette::Inactive, QPalette::Text ) );
        }
                
        p.drawText( x, -1, w, height(), Qt::AlignCenter, tabText( i ) );
    }
    
    const int h = height() - 1;
    p.setPen( QPen( Qt::black, 0 ) );
    p.setRenderHint( QPainter::Antialiasing, false );
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
    p.setBrush( QPalette::Window, QBrush( 0x0e0e0e ) );
    setPalette( p );
    
    connect( m_bar, SIGNAL(currentChanged( int )), m_stack, SLOT(setCurrentIndex( int )) );
    connect( m_bar, SIGNAL(currentChanged( int )), SIGNAL(currentChanged(int)));
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

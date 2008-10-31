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
#ifdef Q_WS_MAC
    QFont f = font();
    f.setPointSize( 11 );
    setFont( f );
#endif

    m_inactive = QPixmap(":/controls/inactive/tab.png");
    m_active = QPixmap(":/controls/active/tab.png");

    QPalette p = palette();
    p.setColor( QPalette::Active, QPalette::Text, Qt::black );
    p.setColor( QPalette::Inactive, QPalette::Text, QColor( 42, 42, 42 ) );
    setPalette( p );
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
    
    setCurrentIndex( e->pos().x() / (width() / count()) );
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
Unicorn::TabBar::succombToTheDarkSide()
{
    m_inactive = QPixmap(":/DockWindow/tab/inactive.png");
    m_active = QPixmap(":/DockWindow/tab/active.png");

    QFont f = font();
    f.setPointSize( 12 );
    f.setBold( false );
    setFont( f );
    
    QPalette p = palette();
    p.setColor( QPalette::Active, QPalette::Text, Qt::white );
    p.setColor( QPalette::Inactive, QPalette::Text, Qt::white );
    setPalette( p );
}


void
Unicorn::TabBar::paintEvent( QPaintEvent* )
{
    QPainter p( this );
    p.fillRect( rect(), m_inactive );
        
    int w = width() / count();
    for (int i = 0; i < count(); ++i)
    {
        int const x = i*w;
        
        if (i == count() - 1)
            w += width() % w;
        
        if (currentIndex() == i)
        {
            p.fillRect( x, 0, w, height(), m_active );
            p.setPen( palette().color( QPalette::Active, QPalette::Text ) );
        }
        else
            p.setPen( palette().color( QPalette::Inactive, QPalette::Text ) );
                
        p.drawText( x, 0, w, height(), Qt::AlignCenter, tabText( i ) );
    }
    
    const int h = height() - 1;
    p.setPen( QColor( 29, 28, 28 ) );
    p.drawLine( 0, h, width(), h );    
}


Unicorn::TabWidget::TabWidget()
{
    QVBoxLayout* v = new QVBoxLayout( this );
    v->addWidget( m_bar = new TabBar );
    v->addWidget( m_stack = new QStackedWidget );
    v->setSpacing( 0 );
    v->setMargin( 0 );
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

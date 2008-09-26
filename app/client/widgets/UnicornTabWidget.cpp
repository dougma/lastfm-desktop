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
#include <QPainter>
#include <QTabBar>
#include <QVBoxLayout>


Unicorn::TabBar::TabBar()
{
#ifdef Q_WS_MAC
    QFont f = font();
    f.setPixelSize( 11 );
    setFont( f );
#endif
}


void
Unicorn::TabBar::paintEvent( QPaintEvent* e )
{
    QPainter p( this );
    p.fillRect( rect(), QBrush( QPixmap(":/controls/inactive/tab.png") ) );

    QFont f = p.font();
    f.setBold( true );
    p.setFont( f );
    
    int w = width() / count();
    for (int i = 0; i < count(); ++i)
    {
        int const x = i*w;
        
        if (i == count() - 1)
            w += width() % w;
        
        if (currentIndex() == i)
        {
            p.fillRect( x, 0, w, height(), QBrush( QPixmap(":/controls/active/tab.png") ) );
            p.setPen( Qt::white );
        }
        else
            p.setPen( QColor( 42, 42, 42 ) );
        
        p.drawText( x, 0, w, height(), Qt::AlignCenter, tabText( i ) );
    }
}


Unicorn::TabWidget::TabWidget()
{
    QVBoxLayout* v = new QVBoxLayout( this );
    v->addWidget( bar = new TabBar );
    v->addWidget( stack = new QStackedWidget );
    v->setSpacing( 0 );
    v->setMargin( 0 );
    connect( bar, SIGNAL(currentChanged( int )), stack, SLOT(setCurrentIndex( int )) );
}


void
Unicorn::TabWidget::addTab( const QString& title, QWidget* w )
{
    bar->addTab( title );
    stack->addWidget( w );
}


void 
Unicorn::TabWidget::setTabEnabled( int index, bool b )
{
    bar->setTabEnabled( index, b );
}

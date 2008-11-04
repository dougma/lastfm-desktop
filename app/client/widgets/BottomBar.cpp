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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "BottomBar.h"
#include "widgets/ImageButton.h"
#include "widgets/ScrobbleButton.h"
#include <QEvent>
#include <QHBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include "the/mainWindow.h"


BottomBar::BottomBar()
{
    QHBoxLayout* h = new QHBoxLayout( this );
    h->addWidget( ui.radio = new BottomBarButton );
    h->addWidget( ui.friends = new BottomBarButton );
    h->addWidget( ui.library = new BottomBarButton );
    h->addSpacing( 12 );
    h->addStretch();
    h->addWidget( ui.scrobble = new ScrobbleButton );
    h->setSpacing( 0 );
    h->setContentsMargins( 8, 3, 4, 4 );
    
    setFixedHeight( sizeHint().height() );

	ui.radio->setCheckable( true );
	ui.radio->setBackgroundPixmap( ":/BottomBar/button/left/up.png" );
	ui.radio->setBackgroundPixmap( ":/BottomBar/button/left/down.png", QIcon::Active );
	ui.radio->setPixmap( ":/BottomBar/icon/radio/off.png" );
	ui.radio->setPixmap( ":/BottomBar/icon/radio/on.png", QIcon::On );
	ui.radio->moveIcon( 1, 0 );
	ui.radio->moveIcon( 1, 1, QIcon::Active );
	
	ui.friends->setCheckable( true );
	ui.friends->setBackgroundPixmap( ":/BottomBar/button/middle/up.png" );
	ui.friends->setBackgroundPixmap( ":/BottomBar/button/middle/down.png", QIcon::Active );
	ui.friends->setPixmap( ":/BottomBar/icon/friends/off.png" );
	ui.friends->setPixmap( ":/BottomBar/icon/friends/on.png", QIcon::On );
	ui.friends->moveIcon( 0, -1 );
	ui.friends->moveIcon( 0, 1, QIcon::Active );
	
	ui.library->setCheckable( true );
	ui.library->setBackgroundPixmap( ":/BottomBar/button/right/up.png" );
	ui.library->setBackgroundPixmap( ":/BottomBar/button/right/down.png", QIcon::Active );
	ui.library->setPixmap( ":/BottomBar/icon/info/off.png" );
	ui.library->setPixmap( ":/BottomBar/icon/info/on.png", QIcon::On );
	ui.library->moveIcon( -1, 0 );
	ui.library->moveIcon( -1, 1, QIcon::Active );

	QLinearGradient g;
	g.setColorAt( 0.0f, QColor( 47, 47, 47 ));
	g.setColorAt( 1.0f, QColor( 26, 26, 26 ));
	g.setStart( 0, 0 );
	g.setFinalStop( 0, height() );

    QPalette p = palette();
    p.setBrush( QPalette::Window, g );
    setPalette( p );
    setAutoFillBackground( true );
}


BottomBarButton::BottomBarButton()
{
    connect( this, SIGNAL(toggled( bool )), SLOT(onToggled( bool )) );
}


void
BottomBarButton::onToggled( bool b )
{   
    m_widget->setVisible( b );
}


void
BottomBarButton::setWidget( QWidget* w )
{
    m_widget = w;
    w->installEventFilter( this );
}


bool
BottomBarButton::eventFilter( QObject* o, QEvent* e )
{
    if (o == m_widget)
        switch ((int)e->type())
        {
            case QEvent::Show:
                setChecked( true );
                break;
            case QEvent::Hide:
                setChecked( false );
                break;
        }
    
    return false;
}
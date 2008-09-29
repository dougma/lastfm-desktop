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

#include "Launcher.h"
#include "widgets/ImageButton.h"
#include "widgets/ScrobbleButton.h"
#include <QEvent>
#include <QHBoxLayout>
#include <QPainter>


Launcher::Launcher()
{
    QHBoxLayout* h = new QHBoxLayout( this );
    h->addWidget( ui.radio = new LauncherButton );
    h->addWidget( ui.friends = new LauncherButton );
    h->addWidget( ui.library = new LauncherButton );
    h->addSpacing( 12 );
    h->addStretch();
    h->addWidget( ui.scrobble = new ScrobbleButton );
    h->setSpacing( 0 );
    h->setContentsMargins( 8, 3, 4, 4 );
	
	connect( ui.radio, SIGNAL( clicked()), SLOT( onRadioToggle()));
	ui.radio->setCheckable( true );
	ui.radio->setBackgroundPixmap( ":/MainWindow/launcher_button_left_rest.png" );
	ui.radio->setBackgroundPixmap( ":/MainWindow/launcher_button_left_pressed.png", QIcon::Active );
	ui.radio->setPixmap( ":/MainWindow/radio_off.png" );
	ui.radio->setPixmap( ":/MainWindow/radio_on.png", QIcon::On );
	ui.radio->moveIcon( 1, 0 );
	ui.radio->moveIcon( 1, 1, QIcon::Active );
	
	ui.friends->setCheckable( true );
	ui.friends->setBackgroundPixmap( ":/MainWindow/launcher_button_centre_rest.png" );
	ui.friends->setBackgroundPixmap( ":/MainWindow/launcher_button_centre_pressed.png", QIcon::Active );
	ui.friends->setPixmap( ":/MainWindow/friends_off.png" );
	ui.friends->setPixmap( ":/MainWindow/friends_on.png", QIcon::On );
	ui.friends->moveIcon( 0, -1 );
	ui.friends->moveIcon( 0, 1, QIcon::Active );
	
	ui.library->setCheckable( true );
	ui.library->setBackgroundPixmap( ":/MainWindow/launcher_button_right_rest.png" );
	ui.library->setBackgroundPixmap( ":/MainWindow/launcher_button_right_pressed.png", QIcon::Active );
	ui.library->setPixmap( ":/MainWindow/info_off.png" );
	ui.library->setPixmap( ":/MainWindow/info_on.png", QIcon::On );
	ui.library->moveIcon( -1, 0 );
	ui.library->moveIcon( -1, 1, QIcon::Active );
}


void
Launcher::paintEvent( QPaintEvent* )
{
	QLinearGradient gradient;
	gradient.setColorAt( 0.0f, QColor( 47, 47, 47 ));
	gradient.setColorAt( 1.0f, QColor( 26, 26, 26 ));
	
	gradient.setStart(rect().width()/2, rect().top());
	gradient.setFinalStop(rect().width()/2, rect().bottom());
	
	QPainter p( this );
	
	p.fillRect( rect(), gradient);
}


LauncherButton::LauncherButton()
{
    connect( this, SIGNAL(toggled( bool )), SLOT(onToggled( bool )) );
}


void
LauncherButton::onToggled( bool b )
{   
    m_widget->setVisible( b );
}


void
LauncherButton::setWidget( QWidget* w )
{
    m_widget = w;
    w->installEventFilter( this );
}


bool
LauncherButton::eventFilter( QObject* o, QEvent* e )
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

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

#include "Launcher.h"
#include "the/mainWindow.h"
#include "radio/RadioWidget.h"
#include <QPainter>

Launcher::Launcher( QWidget* parent )
		 :QWidget( parent )
{
	ui.setupUi( this );
	connect( ui.radio, SIGNAL( clicked()), SLOT( onRadioToggle()));
	ui.radio->setIcon( ":/MainWindow/radio_rest.png" );
	ui.friends->setIcon( ":/MainWindow/friends_rest.png" );
	ui.library->setIcon( ":/MainWindow/library_rest.png" );
	
	ui.scrobble->setCheckable( true );
	ui.scrobble->setIcon( ":/MainWindow/scrobbling_on.png" );
	ui.scrobble->setCheckedIcon( ":MainWindow/scrobbling_off.png" );
}


void 
Launcher::onRadioToggle()
{
	RadioWidget& tuner = *The::mainWindow().ui.tuner;
	if( tuner.isVisible())
	{
	    tuner.hide();
	}
	else
	{
		tuner.move( The::mainWindow().pos().x() + The::mainWindow().width(), 
					The::mainWindow().pos().y());
	    tuner.show();
	}
}


void
Launcher::paintEvent( QPaintEvent* e )
{
	QLinearGradient gradient;
	gradient.setColorAt( 0.0f, QColor( 47, 47, 47 ));
//	gradient.setColorAt( 0.5f, QColor( 16, 16, 16 ) );
	gradient.setColorAt( 1.0f, QColor( 26, 26, 26 ));
	
	gradient.setStart(rect().width()/2, rect().top());
	gradient.setFinalStop(rect().width()/2, rect().bottom());
	
	QPainter p( this );
	
	p.fillRect( rect(), gradient);
}
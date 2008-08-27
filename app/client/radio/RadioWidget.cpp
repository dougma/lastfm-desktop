/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *    This program is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include "RadioWidget.h"
#include "NowPlayingTuner.h"
#include "FriendsTuner.h"
#include "MyTagsTuner.h"
#include "NeighboursTuner.h"
#include "the/radio.h"
#include "MyStations.h"
#include <QAction>
#include <QEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMovie>
#include <QToolBar>
#include <QVBoxLayout>
#include <QSplitter>
#include <QTabWidget>


RadioWidget::RadioWidget()
{
	setLayout( new QHBoxLayout );
	
	ui.tabWidget = new QTabWidget;
	
	#define TUNER(Class, Name) { \
		Class* a##Class = new Class; \
		connect( a##Class, SIGNAL(tune( const RadioStation&)), SLOT(onTune( const RadioStation&)) ); \
		ui.tabWidget->addTab( a##Class, Name ); }

	TUNER( NowPlayingTuner, "Now Playing" );
	TUNER( FriendsTuner, "My Friends" );
	TUNER( MyTagsTuner, "My Tags" );
	TUNER( NeighboursTuner, "My Neighbours" );
	
	#undef TUNER

	QSplitter* s = new QSplitter( Qt::Vertical, this );
	s->addWidget( ui.tabWidget );

	MyStations* myStations = new MyStations;
	connect( myStations, SIGNAL(searchResultComplete( QWidget*, const QString )), SLOT( addTab( QWidget*, const QString )));
	s->addWidget( myStations );
	
	//FIXME: should not rely on hard coded sizes
	//		 but I can't be dealing with getting
	//		 sizing policies working right now!
	s->setSizes( QList<int>() << 373 << 98 );

	layout()->addWidget( s );
	
    setWindowTitle( tr("Last.fm Radio") );

}


void
RadioWidget::onTune( const RadioStation& r )
{
    The::radio().play( r );
}


void
RadioWidget::addTab( QWidget* w, const QString s )
{
	ui.tabWidget->addTab( w, s );
	ui.tabWidget->setCurrentWidget( w );
}
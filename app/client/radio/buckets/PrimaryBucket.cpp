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

#include <QListWidget>
#include <QSplitter>
#include <QTabWidget>
#include <QStackedWidget>
#include <QStackedLayout>
#include "ImageButton.h"
#include "widgets/UnicornWidget.h"
#include <QStringListModel>
#include "PrimaryBucket.h"
#include "PlayerBucket.h"
#include "lib/types/User.h"


PrimaryBucket::PrimaryBucket( QWidget* w )
			  :QMainWindow( w )
{

	UnicornWidget::paintItBlack( this );
	
	ui.friendsBucket = new PrimaryListView( this );
	ui.friendsBucket->setViewMode( QListView::IconMode );
	ui.friendsBucket->setFlow( QListView::LeftToRight );
	ui.friendsBucket->setWrapping( true );
	ui.friendsBucket->setResizeMode( QListView::Adjust );
	
	ui.tagsBucket = new PrimaryListView( this );
	ui.tagsBucket->setViewMode( QListView::IconMode );
	ui.tagsBucket->setFlow( QListView::LeftToRight );
	ui.tagsBucket->setWrapping( true );
	ui.tagsBucket->setResizeMode( QListView::Adjust );
	
#define ADDITEM( b, n, i ) \
	QListWidgetItem* n = new QListWidgetItem( QIcon( i ), #n); \
	n->setSizeHint( QSize( 75, 75)); \
	b->addItem( n );

	ADDITEM( ui.friendsBucket, irvinebrown, ":buckets/irvinebrown.jpg" );
	ADDITEM( ui.friendsBucket, mxcl, ":buckets/mxcl.png" );
	ADDITEM( ui.friendsBucket, musicmobs, ":buckets/musicmobs.jpg" );
	ADDITEM( ui.friendsBucket, dougma, ":buckets/dougma.jpg" );
	ADDITEM( ui.friendsBucket, sharevari, ":buckets/sharevari.png" );

	ADDITEM( ui.tagsBucket, Rock, ":buckets/tag_white_on_blue.png" );
	ADDITEM( ui.tagsBucket, Jazz, ":buckets/tag_white_on_blue.png" );
	ADDITEM( ui.tagsBucket, Metal, ":buckets/tag_white_on_blue.png" );
	ADDITEM( ui.tagsBucket, Folk, ":buckets/tag_white_on_blue.png" );
	
#undef ADDITEM
	
	QSplitter* splitter = new QSplitter( Qt::Horizontal );

	ui.tabWidget = new QTabWidget();

	ui.tabWidget->addTab( ui.friendsBucket, "Your Friends" );
	ui.tabWidget->addTab( ui.tagsBucket, "Your Tags" );
	
	splitter->addWidget( ui.tabWidget );
	
	PlayerBucket* pb;
	splitter->addWidget( pb = new PlayerBucket( this ) );
	
	setCentralWidget( splitter );
}

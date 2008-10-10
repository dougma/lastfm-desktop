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
#include "UnicornTabWidget.h"
#include <QStackedWidget>
#include <QStackedLayout>
#include "widgets/ImageButton.h"
#include "widgets/UnicornWidget.h"
#include <QStringListModel>
#include "PrimaryBucket.h"
#include "PlayerBucket.h"
#include "PlayableListItem.h"
#include "lib/lastfm/types/User.h"


PrimaryBucket::PrimaryBucket()
{
	ui.friendsBucket = new PrimaryListView( this );
    ui.friendsBucket->setAlternatingRowColors( true );
//	ui.friendsBucket->setViewMode( QListView::IconMode );
//	ui.friendsBucket->setFlow( QListView::LeftToRight );
//	ui.friendsBucket->setWrapping( true );
//	ui.friendsBucket->setResizeMode( QListView::Adjust );
    ui.friendsBucket->setDragEnabled( true );
    ui.friendsBucket->setAttribute( Qt::WA_MacShowFocusRect, false );
    UnicornWidget::paintItBlack( ui.friendsBucket );    //on mac, qt 4.4.1 child widgets aren't inheritting palletes properly
    
	ui.tagsBucket = new PrimaryListView( this );
    ui.tagsBucket->setAlternatingRowColors( true );
//	ui.tagsBucket->setViewMode( QListView::IconMode );
//	ui.tagsBucket->setFlow( QListView::LeftToRight );
//	ui.tagsBucket->setWrapping( true );
//	ui.tagsBucket->setResizeMode( QListView::Adjust );
    ui.tagsBucket->setDragEnabled( true );
    ui.tagsBucket->setAttribute( Qt::WA_MacShowFocusRect, false );
    UnicornWidget::paintItBlack( ui.tagsBucket );    //as above
	
	ui.stationsBucket = new PrimaryListView( this );
    ui.stationsBucket->setAlternatingRowColors( true );
//	ui.stationsBucket->setViewMode( QListView::IconMode );
//	ui.stationsBucket->setFlow( QListView::LeftToRight );
//	ui.stationsBucket->setWrapping( true );
//	ui.stationsBucket->setResizeMode( QListView::Adjust );
    ui.stationsBucket->setDragEnabled( true );
    ui.stationsBucket->setAttribute( Qt::WA_MacShowFocusRect, false );
    UnicornWidget::paintItBlack( ui.stationsBucket );    //as above
    
#define ADDITEM( b, n, i ) \
	PlayableListItem* n = new PlayableListItem( QIcon( i ), #n, b ); \
	n->setSizeHint( QSize( 75, 25));
	
#define ADDUSER( b, n, i ) \
	ADDITEM( b, n, i ) \
	n->setType( PlayableMimeData::UserType );

	ADDUSER( ui.friendsBucket, irvinebrown, ":buckets/irvinebrown.jpg" );
	ADDUSER( ui.friendsBucket, mxcl, ":buckets/mxcl.png" );
	ADDUSER( ui.friendsBucket, musicmobs, ":buckets/musicmobs.jpg" );
	ADDUSER( ui.friendsBucket, dougma, ":buckets/dougma.jpg" );
	ADDUSER( ui.friendsBucket, sharevari, ":buckets/sharevari.png" );
#undef ADDUSER
	
#define ADDTAG( b, n, i ) \
	ADDITEM( b, n, i ) \
	n->setType( PlayableMimeData::TagType );
	
	ADDTAG( ui.tagsBucket, Rock, ":buckets/tag_white_on_blue.png" );
	ADDTAG( ui.tagsBucket, Jazz, ":buckets/tag_white_on_blue.png" );
	ADDTAG( ui.tagsBucket, Metal, ":buckets/tag_white_on_blue.png" );
	ADDTAG( ui.tagsBucket, Folk, ":buckets/tag_white_on_blue.png" );

#undef ADDTAG	

#undef ADDITEM
	
	QSplitter* splitter = new QSplitter( Qt::Horizontal );

	ui.tabWidget = new Unicorn::TabWidget;

    ui.tabWidget->addTab( "Your Stations", ui.stationsBucket );
	ui.tabWidget->addTab( "Your Friends", ui.friendsBucket );
	ui.tabWidget->addTab( "Your Tags", ui.tagsBucket );
	
	splitter->addWidget( ui.tabWidget );
	
	splitter->addWidget( ui.playerBucket = new PlayerBucket( this ) );
	
	UnicornWidget::paintItBlack( this );
    
    setCentralWidget( splitter );
}


void 
PrimaryBucket::replaceStation( QMimeData* data )
{
	ui.playerBucket->clear();
	ui.playerBucket->addFromMimeData( data );
}
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
#include <QStackedWidget>
#include <QStackedLayout>
#include "widgets/ImageButton.h"
#include "widgets/UnicornTabWidget.h"
#include "widgets/UnicornWidget.h"
#include <QStringListModel>
#include "PrimaryBucket.h"
#include "PlayerBucket.h"
#include "PlayableListItem.h"
#include "widgets/RadioMiniControls.h"
#include "lib/lastfm/types/User.h"
#include "lib/lastfm/ws/WsReply.h"
#include "lib/lastfm/ws/WsAccessManager.h"
#include "the/radio.h"


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
    
    AuthenticatedUser user;
    
    connect( user.getFriends(), SIGNAL(finished( WsReply* )), SLOT(onUserGetFriendsReturn( WsReply* )) );
    connect( user.getTopTags(), SIGNAL(finished( WsReply* )), SLOT(onUserGetTopTagsReturn( WsReply* )) );
	
	QSplitter* splitter = new QSplitter( Qt::Horizontal );

	ui.tabWidget = new Unicorn::TabWidget;

    ui.tabWidget->addTab( "Your Stations", ui.stationsBucket );
	ui.tabWidget->addTab( "Your Friends", ui.friendsBucket );
	ui.tabWidget->addTab( "Your Tags", ui.tagsBucket );
	
	splitter->addWidget( ui.tabWidget );
	
    QWidget* playerPane =  new QWidget( this );
    new QVBoxLayout( playerPane );
    playerPane->layout()->addWidget( ui.playerBucket = new PlayerBucket( playerPane ) );
    playerPane->layout()->addWidget( ui.controls = new RadioMiniControls);
    UnicornWidget::paintItBlack( ui.controls );

    connect( ui.controls->ui.skip, SIGNAL(clicked()), &The::radio(), SLOT(skip()) );
    connect( ui.controls, SIGNAL( stop()), &The::radio(), SLOT( stop()));
    connect( ui.controls, SIGNAL( stop()), ui.playerBucket, SLOT( clear()));
    connect( ui.controls, SIGNAL( play()), ui.playerBucket, SLOT( play()));
    
    ui.controls->show();
    
	splitter->addWidget( playerPane );
	
	UnicornWidget::paintItBlack( this );
    
    setCentralWidget( splitter );
}


void 
PrimaryBucket::onUserGetFriendsReturn( WsReply* r )
{
    static WsAccessManager* nam = 0;
    if (!nam) nam = new WsAccessManager;
    
    QList< User > users = User::list( r );
    
    foreach( User user, users )
    {
        PlayableListItem* n = new PlayableListItem( user , ui.friendsBucket );
        
        QNetworkReply* r = nam->get( QNetworkRequest( user.mediumImageUrl()));
        connect( r, SIGNAL( finished()), n, SLOT( iconDataDownloaded()));
        
        n->setSizeHint( QSize( 75, 25));
        n->setType( PlayableMimeData::UserType );
    }
}


void 
PrimaryBucket::onUserGetTopTagsReturn( WsReply* r )
{
    WeightedStringList tags = Tag::list( r );
    
    foreach( WeightedString tag, tags )
    {
        PlayableListItem* n = new PlayableListItem( tag, ui.tagsBucket );
        n->setIcon( QIcon( ":buckets/tag_white_on_blue.png" ) );
        n->setSizeHint( QSize( 75, 25));
        n->setType( PlayableMimeData::TagType );
    }
}


void 
PrimaryBucket::replaceStation( QMimeData* data )
{
	ui.playerBucket->clear();
	ui.playerBucket->addFromMimeData( data );
}
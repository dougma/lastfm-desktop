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
#include <QLineEdit>
#include <QComboBox>
#include "widgets/ImageButton.h"
#include "widgets/UnicornTabWidget.h"
#include "widgets/UnicornWidget.h"
#include <QStringListModel>
#include "PrimaryBucket.h"
#include "PlayerBucket.h"
#include "PlayableListItem.h"
#include "DelegateDragHint.h"
#include "widgets/RadioControls.h"
#include "lib/lastfm/types/User.h"
#include "lib/lastfm/ws/WsReply.h"
#include "lib/lastfm/ws/WsAccessManager.h"
#include "the/radio.h"

#include <Phonon/volumeslider.h>

Q_DECLARE_METATYPE( PlayableMimeData::Type )

PrimaryBucket::PrimaryBucket()
{
    ui.friendsBucket = new PrimaryListView( this );
    ui.friendsBucket->setAlternatingRowColors( true );
    ui.friendsBucket->setDragEnabled( true );
    ui.friendsBucket->setAttribute( Qt::WA_MacShowFocusRect, false );
    connect( ui.friendsBucket, SIGNAL( doubleClicked(const QModelIndex&)), SLOT( onItemDoubleClicked( const QModelIndex&)));
    UnicornWidget::paintItBlack( ui.friendsBucket );    //on mac, qt 4.4.1 child widgets aren't inheritting palletes properly
    
	ui.tagsBucket = new PrimaryListView( this );
    ui.tagsBucket->setAlternatingRowColors( true );
    ui.tagsBucket->setDragEnabled( true );
    ui.tagsBucket->setAttribute( Qt::WA_MacShowFocusRect, false );
    connect( ui.tagsBucket, SIGNAL( doubleClicked(const QModelIndex&)), SLOT( onItemDoubleClicked( const QModelIndex&)));
    UnicornWidget::paintItBlack( ui.tagsBucket );    //as above
	
	ui.stationsBucket = new PrimaryListView( this );
    ui.stationsBucket->setAlternatingRowColors( true );
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
    
    QWidget* primaryPane = new QWidget( this );
    new QVBoxLayout( primaryPane );
    primaryPane->layout()->addWidget( ui.tabWidget );
    
    QWidget* freeInputWidget = new QWidget( this );
    new QHBoxLayout( freeInputWidget );
    freeInputWidget->layout()->addWidget( ui.freeInput = new QLineEdit );
    ui.freeInput->setAttribute( Qt::WA_MacShowFocusRect, false );
    
    freeInputWidget->layout()->addWidget( ui.inputSelector = new QComboBox );
    ui.inputSelector->addItem( "Artist", QVariant::fromValue(PlayableMimeData::ArtistType) );
    ui.inputSelector->addItem( "Tag", QVariant::fromValue(PlayableMimeData::TagType) );
    ui.inputSelector->addItem( "User", QVariant::fromValue(PlayableMimeData::UserType) );
    
    primaryPane->layout()->addWidget( freeInputWidget );

	splitter->addWidget( primaryPane );
	
    QWidget* playerPane =  new QWidget( this );
    new QVBoxLayout( playerPane );
    playerPane->layout()->addWidget( ui.playerBucket = new PlayerBucket( playerPane ) );
    playerPane->layout()->addWidget( ui.controls = new RadioControls);
    playerPane->setAutoFillBackground( true );
    ui.controls->ui.volume->setAudioOutput( The::radio().audioOutput() );
    
    connect( ui.controls->ui.skip, SIGNAL(clicked()), &The::radio(), SLOT(skip()) );
    connect( ui.controls, SIGNAL( stop()), &The::radio(), SLOT( stop()));
    connect( ui.controls, SIGNAL( stop()), ui.playerBucket, SLOT( clear()));
    connect( ui.controls, SIGNAL( play()), ui.playerBucket, SLOT( play()));
    connect( ui.freeInput, SIGNAL( returnPressed()), SLOT( onFreeInputReturn()));
    
    ui.controls->show();
    
	splitter->addWidget( playerPane );
    UnicornWidget::paintItBlack( splitter );
    splitter->setAutoFillBackground( true );
	
    setCentralWidget( splitter );
    
	UnicornWidget::paintItBlack( this );
}


void
PrimaryBucket::onFreeInputReturn()
{
    PlayableMimeData::Type type = ui.inputSelector->itemData(ui.inputSelector->currentIndex()).value<PlayableMimeData::Type>();
    ui.playerBucket->addAndLoadItem( ui.freeInput->text(), type );
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
PrimaryBucket::onItemDoubleClicked( const QModelIndex& index )
{
    PrimaryListView* itemView = dynamic_cast< PrimaryListView*>( sender() );
    Q_ASSERT( itemView );
    
    QStyleOptionViewItem options;
    options.initFrom( this );
    options.decorationSize = itemView->iconSize().expandedTo( QSize( 16, 16) );
    options.displayAlignment = Qt::AlignVCenter | Qt::AlignLeft;
    options.decorationAlignment = Qt::AlignVCenter | Qt::AlignCenter;
    options.rect = itemView->visualRect( index );
    DelegateDragHint* w = new DelegateDragHint( itemView->itemDelegate( index ), index, options, itemView );
    w->setMimeData( itemView->mimeData( QList<QListWidgetItem*>()<< itemView->itemFromIndex(index) ) );
    w->dragTo( ui.playerBucket );
    connect( w, SIGNAL( finishedAnimation()), SLOT( onDnDAnimationFinished()));
}


void 
PrimaryBucket::onDnDAnimationFinished()
{
    DelegateDragHint* delegateWidget = static_cast<DelegateDragHint*>(sender());
    QModelIndex index = delegateWidget->index();
}
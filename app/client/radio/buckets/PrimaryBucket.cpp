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

#include <phonon/volumeslider.h>

Q_DECLARE_METATYPE( Seed::Type )


struct SpecialSplitter : public QSplitter
{
    SpecialSplitter( Qt::Orientation o ) : QSplitter( o )
    {}

    struct cHandle : public QSplitterHandle
    {
        cHandle( QSplitter* parent ) : QSplitterHandle( Qt::Horizontal, parent )
        {}
        
        virtual void paintEvent( QPaintEvent* )
        {
            QPainter p( this );
            p.fillRect( rect(), QColor( 35, 35, 35 ) );
            p.setPen( QColor( 23, 23, 24 ) );
            p.drawLine( 0, 0, 0, height() );
        }        
    };
    
    virtual QSplitterHandle* createHandle() { return new cHandle( this ); }
};


struct SpecialWidget : QWidget
{
    virtual void paintEvent( QPaintEvent* )
    {
        QPainter p( this );
        p.fillRect( rect(), QColor( 35, 35, 35 ) );
        p.setPen( QColor( 27, 27, 27 ) );
        p.drawLine( 0, 0, width(), 0 );
        p.setPen( QColor( 55, 55, 55 ) );
        p.drawLine( 0, 1, width(), 1 );
    }    
};


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

#if 0        
    QString const name = AuthenticatedUser().name();
    foreach (QString url, QStringList() << "My Library Radio" << "My Loved Tracks Radio" << "My Neighbourhood Radio" )
    {
        PlayableListItem* n = new PlayableListItem( url, ui.stationsBucket );
        n->setPixmap( QPixmap( ":/BottomBar/icon/radio/on.png" ) );
        n->setSizeHint( QSize( 75, 25) );
        n->setType( Seed::StationType );
    }
#endif

    AuthenticatedUser user;
    
    connect( user.getFriends(), SIGNAL(finished( WsReply* )), SLOT(onUserGetFriendsReturn( WsReply* )) );
    connect( user.getTopTags(), SIGNAL(finished( WsReply* )), SLOT(onUserGetTopTagsReturn( WsReply* )) );
	
	QSplitter* splitter = new SpecialSplitter( Qt::Horizontal );

	ui.tabWidget = new Unicorn::TabWidget;

    ui.tabWidget->addTab( "Your Stations", ui.stationsBucket );
	ui.tabWidget->addTab( "Your Friends", ui.friendsBucket );
	ui.tabWidget->addTab( "Your Tags", ui.tagsBucket );
    ui.tabWidget->bar()->succombToTheDarkSide();
    ui.tabWidget->bar()->setMinimumHeight( 26 );
    
    QWidget* primaryPane = new QWidget( this );
    new QVBoxLayout( primaryPane );
    primaryPane->layout()->addWidget( ui.tabWidget );
    primaryPane->layout()->setMargin( 0 );
    primaryPane->layout()->setSpacing( 0 );
    
    QWidget* freeInputWidget = new SpecialWidget;
    new QHBoxLayout( freeInputWidget );
    freeInputWidget->layout()->addWidget( ui.freeInput = new QLineEdit );
    ui.freeInput->setAttribute( Qt::WA_MacShowFocusRect, false );
    
    freeInputWidget->layout()->addWidget( ui.inputSelector = new QComboBox );
    ui.inputSelector->addItem( "Artist", QVariant::fromValue(Seed::ArtistType) );
    ui.inputSelector->addItem( "Tag", QVariant::fromValue(Seed::TagType) );
    ui.inputSelector->addItem( "User", QVariant::fromValue(Seed::UserType) );
    ui.inputSelector->setAttribute( Qt::WA_MacNormalSize ); //wtf? needed tho, when floating
    
    primaryPane->layout()->addWidget( freeInputWidget );

	splitter->addWidget( primaryPane );
	
    QWidget* playerPane =  new QWidget;
    new QVBoxLayout( playerPane );
    playerPane->layout()->addWidget( ui.playerBucket = new PlayerBucket( playerPane ) );
    playerPane->layout()->addWidget( ui.controls = new RadioControls);
    playerPane->layout()->setContentsMargins( 0, 7, 7, 0 );
    playerPane->layout()->setSpacing( 0 );
    
    playerPane->setAutoFillBackground( true );
    ui.controls->ui.volume->setAudioOutput( The::radio().audioOutput() );
    
    connect( ui.controls->ui.skip, SIGNAL(clicked()), &The::radio(), SLOT(skip()) );
    connect( ui.controls, SIGNAL( stop()), &The::radio(), SLOT( stop()));
    connect( ui.controls, SIGNAL( stop()), ui.playerBucket, SLOT( clear()));
    connect( ui.controls, SIGNAL( play()), ui.playerBucket, SLOT( play()));
    connect( ui.freeInput, SIGNAL( returnPressed()), SLOT( onFreeInputReturn()));
    connect( ui.playerBucket, SIGNAL( itemRemoved( QString, Seed::Type)), SLOT( onPlayerBucketItemRemoved( QString, Seed::Type)));
    
    ui.controls->show();
    
	splitter->addWidget( playerPane );
	
    setCentralWidget( splitter );
    
	UnicornWidget::paintItBlack( this );
}


void
PrimaryBucket::onFreeInputReturn()
{
    Seed::Type type = ui.inputSelector->itemData(ui.inputSelector->currentIndex()).value<Seed::Type>();
    ui.playerBucket->addAndLoadItem( ui.freeInput->text(), type );
    ui.freeInput->clear();
}


void 
PrimaryBucket::onUserGetFriendsReturn( WsReply* r )
{
    static WsAccessManager* nam = new WsAccessManager;
    
    QList< User > users = User::list( r );
    
    foreach( User user, users )
    {
        PlayableListItem* n = new PlayableListItem( user, ui.friendsBucket );
        
        QNetworkReply* r = nam->get( QNetworkRequest( user.mediumImageUrl()));
        connect( r, SIGNAL( finished()), n, SLOT( iconDataDownloaded()));
        
        n->setSizeHint( QSize( 75, 25));
        n->setType( Seed::UserType );
    }
}


void 
PrimaryBucket::onUserGetTopTagsReturn( WsReply* r )
{
    WeightedStringList tags = Tag::list( r );
    
    foreach( WeightedString tag, tags )
    {
        PlayableListItem* n = new PlayableListItem( tag, ui.tagsBucket );
        n->setPixmap( QPixmap( ":/buckets/tag.png" ) );
        n->setSizeHint( QSize( 75, 25));
        n->setType( Seed::TagType );
    }
}


void 
PrimaryBucket::onItemDoubleClicked( const QModelIndex& index )
{
    PrimaryListView* itemView = dynamic_cast< PrimaryListView*>( sender() );
    Q_ASSERT( itemView );

    if( !(itemView->itemFromIndex( index )->flags() & Qt::ItemIsEnabled) )
        return;
    
    QStyleOptionViewItem options;
    options.initFrom( this );
    options.decorationSize = itemView->iconSize().expandedTo( QSize( 16, 16) );
    options.displayAlignment = Qt::AlignVCenter | Qt::AlignLeft;
    options.decorationAlignment = Qt::AlignVCenter | Qt::AlignCenter;
    options.rect = itemView->visualRect( index );
    
    itemView->itemFromIndex( index )->setFlags( Qt::NoItemFlags );
    
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


void 
PrimaryBucket::onPlayerBucketItemRemoved( QString text, Seed::Type type )
{
    switch ( type ) {
        case Seed::UserType :
            foreach( QListWidgetItem* item, ui.friendsBucket->findItems( text, Qt::MatchFixedString ))
            {
                item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled );
            }
            break;
            
        case Seed::TagType:
            foreach( QListWidgetItem* item, ui.tagsBucket->findItems( text, Qt::MatchFixedString ))
            {
                item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled );
            }
            break;
        default:
            break;
    }
}

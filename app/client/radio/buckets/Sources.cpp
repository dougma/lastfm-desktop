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
#include "Sources.h"
#include "Amp.h"
#include "the/mainWindow.h"
#include "PlayableListItem.h"
#include "DelegateDragHint.h"
#include "widgets/RadioControls.h"
#include "lib/lastfm/types/User.h"
#include "lib/lastfm/ws/WsReply.h"
#include "lib/lastfm/ws/WsAccessManager.h"

#include <phonon/volumeslider.h>

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


Sources::Sources()
        :m_connectedAmp( 0 )
{
    m_accessManager = new WsAccessManager( this );
    
    setupUi();
    
    AuthenticatedUser authUser;
     
    QString const name = authUser.name();
    
    typedef QPair<QString, QString> StringPair;
    typedef QList<StringPair > PairList;
    StringPair station;
    foreach( station, PairList() << StringPair( tr("My Library"), QString("library:%1").arg(name))
                                 << StringPair( tr("My Loved Tracks"), QString("loved:%1").arg(name))
                                 << StringPair( tr("My Recommendations"), QString( "recs:%1" ).arg(name)))
                               // FIXME: Neighbours not implemented in RQL yet!
                               //  << StringPair( tr("My Neighbours"), QString( "neighbours:%1" ).arg(name)))

    {
        PlayableListItem* n = new PlayableListItem( station.first, ui.stationsBucket );
        n->setRQL( station.second );
        n->setPlayableType( Seed::PreDefinedType );
        QRect textRect = QFontMetrics( n->font()).boundingRect( station.first );
        n->setSizeHint( QSize( textRect.width() + 20 , 70));
        connect( authUser.getInfo(), SIGNAL( finished( WsReply*)), SLOT( onAuthUserInfoReturn( WsReply* )) );
    }

    connect( authUser.getFriends(), SIGNAL(finished( WsReply* )), SLOT(onUserGetFriendsReturn( WsReply* )) );
    connect( authUser.getTopTags(), SIGNAL(finished( WsReply* )), SLOT(onUserGetTopTagsReturn( WsReply* )) );
    connect( authUser.getPlaylists(), SIGNAL(finished( WsReply* )), SLOT(onUserGetPlaylistsReturn( WsReply* )) );
	
}


void 
Sources::setupUi()
{
    new QVBoxLayout( this );
    layout()->setMargin( 0 );
    layout()->setSpacing( 0 );
    
    ui.tabWidget = new Unicorn::TabWidget;
    ui.tabWidget->bar()->setMinimumHeight( 26 );
    layout()->addWidget( ui.tabWidget );
    
    ui.stationsBucket = new PrimaryListView( this );
    ui.stationsBucket->setAlternatingRowColors( true );
    ui.stationsBucket->setDragEnabled( true );
    ui.stationsBucket->setAttribute( Qt::WA_MacShowFocusRect, false );
    connect( ui.stationsBucket, SIGNAL( doubleClicked(const QModelIndex&)), SLOT( onItemDoubleClicked( const QModelIndex&)));
    UnicornWidget::paintItBlack( ui.stationsBucket );    //on mac, qt 4.4.1 child widgets aren't inheritting palletes properly
    ui.stationsBucket->setAlternatingRowColors( false );
    ui.tabWidget->addTab( "Your Stations", ui.stationsBucket );
    
    ui.friendsBucket = new PrimaryListView( this );
    ui.friendsBucket->setAlternatingRowColors( true );
    ui.friendsBucket->setDragEnabled( true );
    ui.friendsBucket->setAttribute( Qt::WA_MacShowFocusRect, false );
    connect( ui.friendsBucket, SIGNAL( doubleClicked(const QModelIndex&)), SLOT( onItemDoubleClicked( const QModelIndex&)));
    UnicornWidget::paintItBlack( ui.friendsBucket );    //as above
    ui.friendsBucket->setAlternatingRowColors( false );
    ui.tabWidget->addTab( "Your Friends", ui.friendsBucket );
    
	ui.tagsBucket = new PrimaryListView( this );
    ui.tagsBucket->setAlternatingRowColors( true );
    ui.tagsBucket->setDragEnabled( true );
    ui.tagsBucket->setAttribute( Qt::WA_MacShowFocusRect, false );
    connect( ui.tagsBucket, SIGNAL( doubleClicked(const QModelIndex&)), SLOT( onItemDoubleClicked( const QModelIndex&)));
    UnicornWidget::paintItBlack( ui.tagsBucket );    //as above
    ui.tagsBucket->setAlternatingRowColors( false );
    ui.tabWidget->addTab( "Your Tags", ui.tagsBucket );
    
    QWidget* freeInputWidget = new SpecialWidget;
    new QHBoxLayout( freeInputWidget );
    freeInputWidget->layout()->addWidget( ui.freeInput = new QLineEdit );
    ui.freeInput->setAttribute( Qt::WA_MacShowFocusRect, false );
    connect( ui.freeInput, SIGNAL( returnPressed()), SLOT( onFreeInputReturn()));
    
    freeInputWidget->layout()->addWidget( ui.inputSelector = new QComboBox );
    ui.inputSelector->addItem( "Artist", QVariant::fromValue(Seed::ArtistType) );
    ui.inputSelector->addItem( "Tag", QVariant::fromValue(Seed::TagType) );
    ui.inputSelector->addItem( "User", QVariant::fromValue(Seed::UserType) );
    ui.inputSelector->setAttribute( Qt::WA_MacNormalSize ); //wtf? needed tho, when floating
    
    layout()->addWidget( freeInputWidget );
    
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
	UnicornWidget::paintItBlack( this );
    
    
}


void
Sources::onFreeInputReturn()
{
    if( !m_connectedAmp )
        return;
    
    Seed::Type type = ui.inputSelector->itemData(ui.inputSelector->currentIndex()).value<Seed::Type>();

    m_connectedAmp->addAndLoadItem( ui.freeInput->text(), type );
    
    ui.freeInput->clear();
}


void 
Sources::onUserGetFriendsReturn( WsReply* r )
{
    QList< User > users = User::list( r );
    
    foreach( User user, users )
    {
        PlayableListItem* n = new PlayableListItem( user, ui.friendsBucket );
        
        QNetworkReply* r = m_accessManager->get( QNetworkRequest( user.mediumImageUrl()));
        connect( r, SIGNAL( finished()), n, SLOT( iconDataDownloaded()));
        
        n->setSizeHint( QSize( 70, 70));
        n->setPlayableType( Seed::UserType );	
    }
}


void 
Sources::onUserGetTopTagsReturn( WsReply* r )
{
    WeightedStringList tags = Tag::list( r );
    
    foreach( WeightedString tag, tags )
    {
        PlayableListItem* n = new PlayableListItem( tag, ui.tagsBucket );
        n->setSizeHint( QSize( 70, 70));
        n->setPixmap( QPixmap( ":/buckets/tag.png" ) );
        n->setPlayableType( Seed::TagType );
    }
}
            
            
void 
Sources::onUserGetPlaylistsReturn( WsReply* r )
{
#if 0 //FIXME: No RQL for playlists yet!
    QList<CoreDomElement> playlists = r->lfm().children( "playlist" );
    foreach( CoreDomElement playlist, playlists )
    {
        PlayableListItem* n = new PlayableListItem( playlist[ "title" ].text(), ui.stationsBucket );

        QString smallImageUrl = playlist.optional( "image size=small").text();
        if( !smallImageUrl.isEmpty() )
        {
            QNetworkReply* r = m_accessManager->get( QNetworkRequest( smallImageUrl));
            connect( r, SIGNAL( finished()), n, SLOT( iconDataDownloaded()));
        } 
        
        n->setPlayableType( Seed::PreDefinedType );
        //FIXME: this is not yet implemented in RQL
        n->setRQL( "" );
        
    }
#endif
}


void 
Sources::onItemDoubleClicked( const QModelIndex& index )
{
    if( !m_connectedAmp )
        return;
    
    PrimaryListView* itemView = dynamic_cast< PrimaryListView*>( sender() );
    Q_ASSERT( itemView );

    if( !(itemView->itemFromIndex( index )->flags() & Qt::ItemIsEnabled) )
        return;
    
    QStyleOptionViewItem options;
    options.initFrom( itemView );
    options.decorationSize = itemView->iconSize();
    options.displayAlignment = Qt::AlignVCenter | Qt::AlignLeft;
    options.decorationAlignment = Qt::AlignVCenter | Qt::AlignCenter;
    options.rect = itemView->visualRect( index );
    
    itemView->itemFromIndex( index )->setFlags( Qt::NoItemFlags );
    
    DelegateDragHint* w = new DelegateDragHint( itemView->itemDelegate( index ), index, options, itemView );
    w->setMimeData( itemView->mimeData( QList<QListWidgetItem*>()<< itemView->itemFromIndex(index) ) );
    w->dragTo( m_connectedAmp );
    connect( w, SIGNAL( finishedAnimation()), SLOT( onDnDAnimationFinished()));
}


void 
Sources::onDnDAnimationFinished()
{
    DelegateDragHint* delegateWidget = static_cast<DelegateDragHint*>(sender());
    QModelIndex index = delegateWidget->index();
}


void 
Sources::onAmpSeedRemoved( QString text, Seed::Type type )
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
            
        case Seed::PreDefinedType:
            foreach( QListWidgetItem* item, ui.stationsBucket->findItems( text, Qt::MatchFixedString ))
            {
                item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled );
            }
            break;            
        default:
            break;
    }
}


void 
Sources::connectToAmp( Amp* amp )
{
    if( m_connectedAmp )
        m_connectedAmp->disconnect( this );
    
    m_connectedAmp = amp;
    connect( m_connectedAmp, SIGNAL(itemRemoved(QString, Seed::Type)), SLOT( onAmpSeedRemoved(QString, Seed::Type)));
    connect( m_connectedAmp, SIGNAL(destroyed()), SLOT( onAmpDestroyed()));
}


void
Sources::onAuthUserInfoReturn( WsReply* r )
{
    QList<CoreDomElement> images = r->lfm().children( "image" );
    if( images.isEmpty() )
        return;

    QNetworkReply* reply = m_accessManager->get( QNetworkRequest( images.first().text() ));

    connect( reply, SIGNAL( finished()), SLOT( authUserIconDataDownloaded()));
}


void 
Sources::authUserIconDataDownloaded()
{
    QNetworkReply* reply = static_cast< QNetworkReply* >( sender());

    QPixmap pm;
    pm.loadFromData( reply->readAll()); 
    QListWidgetItem* item;
    for( int i = 0; (item = ui.stationsBucket->item( i )); i++ )
    {
        PlayableListItem* pitem;
        if( !(pitem = dynamic_cast< PlayableListItem*>( item )))
            continue;
        
        pitem->setPixmap( pm );
    }
}
/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
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
#include <QPushButton>
#include "widgets/UnicornTabWidget.h"
#include "widgets/UnicornWidget.h"
#include <QStringListModel>
#include "SeedsWidget.h"
#include "SeedListView.h"
#include "Amp.h"
#include "the/mainWindow.h"
#include <lastfm/RadioStation.h"
#include "Seed.h"
#include "SeedListModel.h"
#include "DelegateDragHint>
#include "widgets/RadioControls.h"
#include <lastfm/User>
#include <lastfm/WsReply>
#include <lastfm/WsAccessManager>
#include "widgets/Firehose.h"
#include "radio/buckets/PlayerBucketList.h"
#include "radio/buckets/CombosWidget.h"
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


SeedsWidget::SeedsWidget()
        :m_connectedAmp( 0 )
{
    m_accessManager = new WsAccessManager( this );
    
    setupUi();
    
    AuthenticatedUser authUser;
     
    QString const name = authUser.name();
    
    typedef QPair<QString, QString> StringPair;
    typedef QList<StringPair > PairList;
    StringPair station;
    foreach( station, PairList() << StringPair( tr("Your Library"), QString("library:%1").arg(name))
                                 << StringPair( tr("Your\xa0Loved Tracks"), QString("loved:%1").arg(name))
                                 << StringPair( tr("Recommended"), QString( "recs:%1" ).arg(name)))
                               // FIXME: Neighbours not implemented in RQL yet!
                               //  << StringPair( tr("My Neighbours"), QString( "neighbours:%1" ).arg(name)))

    {
        Seed* n = new Seed( station.first, ui.stationsBucket );
        n->setRQL( station.second );
        n->setPlayableType( Seed::PreDefinedType );
        connect( authUser.getInfo(), SIGNAL( finished( WsReply*)), SLOT( onAuthUserInfoReturn( WsReply* )) );
    }
    
    onTabChanged();

    connect( authUser.getFriends(), SIGNAL(finished( WsReply* )), SLOT(onUserGetFriendsReturn( WsReply* )) );
    connect( authUser.getTopTags(), SIGNAL(finished( WsReply* )), SLOT(onUserGetTopTagsReturn( WsReply* )) );
    connect( authUser.getPlaylists(), SIGNAL(finished( WsReply* )), SLOT(onUserGetPlaylistsReturn( WsReply* )) );
    connect( authUser.getTopArtists(), SIGNAL(finished( WsReply* )), SLOT(onUserGetArtistsReturn( WsReply* )) );
	
}


void 
SeedsWidget::setupUi()
{
    new QVBoxLayout( this );
    layout()->setMargin( 0 );
    layout()->setSpacing( 0 );
    
    ui.tabWidget = new Unicorn::TabWidget;
    ui.tabWidget->bar()->setTearable( true );

    ui.tabWidget->bar()->addWidget( ui.cog = new ImageButton( ":/MainWindow/button/cog/up.png" ) );
    ui.tabWidget->bar()->addWidget( ui.dashboard = new QPushButton( "Now Playing" ));
    connect( ui.cog, SIGNAL(pressed()), SLOT(onCogMenuClicked()) );
    m_cogMenu = new QMenu( this );
    
    connect( ui.tabWidget, SIGNAL( currentChanged( int )), SLOT( onTabChanged()));
    connect( this, SIGNAL( customContextMenuRequested( const QPoint& )), SLOT( onContextMenuRequested( const QPoint& )));
    
    layout()->addWidget( ui.tabWidget );
    
    ui.stationsBucket = new SeedListView( this );
    ui.stationsBucket->setWindowTitle( tr( "Stations" ) );
    connect( ui.stationsBucket, SIGNAL( doubleClicked(const QModelIndex&)), SLOT( onItemDoubleClicked( const QModelIndex&)));
    UnicornWidget::paintItBlack( ui.stationsBucket );    //on mac, qt 4.4.1 child widgets aren't inheritting palletes properly
    ui.tabWidget->addTab( ui.stationsBucket );
    
    ui.friendsBucket = new SeedListView( this );
    ui.friendsBucket->setWindowTitle( tr( "Friends" ));
    
    Firehose* hose;
    ui.friendsBucket->addCustomWidget( hose = new Firehose, tr( "Firehose View" ) );
    ui.friendsBucket->setSourcesViewMode( SeedListView::CustomMode );

    connect( ui.friendsBucket, SIGNAL( doubleClicked(const QModelIndex&)), SLOT( onItemDoubleClicked( const QModelIndex&)));
    UnicornWidget::paintItBlack( ui.friendsBucket );    //as above
    ui.tabWidget->addTab( ui.friendsBucket );
    
	ui.tagsBucket = new SeedListView( this );
    ui.tagsBucket->setWindowTitle( tr( "Tags" ));
    
    connect( ui.tagsBucket, SIGNAL( doubleClicked(const QModelIndex&)), SLOT( onItemDoubleClicked( const QModelIndex&)));
    UnicornWidget::paintItBlack( ui.tagsBucket );    //as above
    ui.tabWidget->addTab( ui.tagsBucket );

	ui.artistsBucket = new SeedListView( this );
    ui.artistsBucket->setWindowTitle( tr( "Artists" ));
    connect( ui.artistsBucket, SIGNAL( doubleClicked(const QModelIndex&)), SLOT( onItemDoubleClicked( const QModelIndex&)));
    UnicornWidget::paintItBlack( ui.artistsBucket );    //as above
    ui.tabWidget->addTab( ui.artistsBucket );
    
    ui.combosWidget = new CombosWidget( this );
    ui.combosWidget->setWindowTitle( tr( "Combos" ));
    UnicornWidget::paintItBlack( ui.combosWidget );
    ui.tabWidget->addTab( ui.combosWidget );
    
#if 0
    //
    //TODO: Uncomment when we have a recentArtists webservice
    //
    
    QAction* topArtists = new QAction( tr( "Top Artists" ), ui.artistsBucket );
    topArtists->setCheckable( true );
    topArtists->setChecked( true );
    
    QAction* recentArtists = new QAction( tr( "Recent Artists" ), ui.artistsBucket );
    recentArtists->setCheckable( true );
    
    QActionGroup* artistGroup = new QActionGroup( ui.artistsBucket );
    artistGroup->addAction( topArtists );
    artistGroup->addAction( recentArtists );
    
    QAction* sep = new QAction( ui.artistsBucket );
    sep->setSeparator( true );

    ui.artistsBucket->addAction( sep );
    ui.artistsBucket->addAction( topArtists );
    ui.artistsBucket->addAction( recentArtists );
    
    connect( topArtists, SIGNAL( toggled( bool)), SLOT( onTopArtistsToggled( bool )));
    connect( recentArtists, SIGNAL( toggled( bool)), SLOT( onRecentArtistsToggled( bool )));
#endif

    
    QWidget* freeInputWidget = new SpecialWidget;
    new QHBoxLayout( freeInputWidget );
    freeInputWidget->layout()->addWidget( ui.freeInput = new QLineEdit );
    ui.freeInput->setAttribute( Qt::WA_MacShowFocusRect, false );
    connect( ui.freeInput, SIGNAL( returnPressed()), SLOT( onFreeInputReturn()));
    
    freeInputWidget->layout()->addWidget( ui.inputSelector = new QComboBox );
    ui.inputSelector->addItem( tr( "Artist" ), QVariant::fromValue(Seed::ArtistType) );
    ui.inputSelector->addItem( tr( "Tag" ), QVariant::fromValue(Seed::TagType) );
    ui.inputSelector->addItem( tr( "User" ), QVariant::fromValue(Seed::UserType) );
    ui.inputSelector->setAttribute( Qt::WA_MacNormalSize ); //wtf? needed tho, when floating
    
    layout()->addWidget( freeInputWidget );
    
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
	UnicornWidget::paintItBlack( this );
    
    
}


void
SeedsWidget::onFreeInputReturn()
{
    if( ui.freeInput->text().trimmed().startsWith( "lastfm://" ))
    {
        RadioStation s( ui.freeInput->text() );
        The::app().open( s );
        ui.freeInput->clear();
        return;
    }
        
    
    if( !m_connectedAmp )
        return;
    
    Seed::Type type = ui.inputSelector->itemData(ui.inputSelector->currentIndex()).value<Seed::Type>();

    m_connectedAmp->addAndLoadItem( ui.freeInput->text(), type );
    
    ui.freeInput->clear();
}


void 
SeedsWidget::onUserGetFriendsReturn( WsReply* r )
{
    QList< User > users = User::list( r );
    
    foreach( User user, users )
    {
        Seed* n = new Seed( user, ui.friendsBucket );
        
        QNetworkReply* r = m_accessManager->get( QNetworkRequest( user.largeImageUrl()));
        connect( r, SIGNAL( finished()), n, SLOT( iconDataDownloaded()));
        
        n->setPlayableType( Seed::UserType );	
    }
    ui.friendsBucket->reset();
}


void 
SeedsWidget::onUserGetTopTagsReturn( WsReply* r )
{
    QStringList tags = Tag::list( r ).values();
    if( tags.isEmpty() )
        return;
    
    QIcon tagIcon;
    tagIcon.addPixmap( QPixmap( ":/buckets/tag_39.png" ) );
    tagIcon.addPixmap( QPixmap( ":/buckets/tag_39.png" ), QIcon::Selected );
    tagIcon.addPixmap( QPixmap( ":/buckets/tag_21.png" ) );
    tagIcon.addPixmap( QPixmap( ":/buckets/tag_21.png" ), QIcon::Selected );
    foreach( QString const tag, tags )
    {
        Seed* n = new Seed( tag, ui.tagsBucket );
        n->setIcon( tagIcon );
        n->setPlayableType( Seed::TagType );
    }
}


void 
SeedsWidget::onUserGetArtistsReturn( WsReply* r )
{
    QList<Artist> artists = Artist::list( r );
    foreach( Artist a, artists )
    {
        Seed* n = new Seed( a, ui.artistsBucket );
        n->setPlayableType( Seed::ArtistType );

        QNetworkReply* r = m_accessManager->get( QNetworkRequest( a.imageUrl() ));
        connect( r, SIGNAL( finished()), n, SLOT( iconDataDownloaded()));
    }
}


void
SeedsWidget::onAuthUserInfoReturn( WsReply* r )
{
    QList<WsDomElement> images = r->lfm().children( "image" );
    if( images.isEmpty() )
        return;
    
    QNetworkReply* reply = m_accessManager->get( QNetworkRequest( images.first().text() ));
    
    connect( reply, SIGNAL( finished()), SLOT( authUserIconDataDownloaded()));
}


void 
SeedsWidget::authUserIconDataDownloaded()
{
    QNetworkReply* reply = static_cast< QNetworkReply* >( sender() );
    
    QPixmap pm;
    if ( pm.loadFromData( reply->readAll()) ) 
    {
        for( int i = 0; i < ui.stationsBucket->seedModel()->rowCount(); i++ )
        {
            ui.stationsBucket->seedModel()->setData( ui.stationsBucket->seedModel()->index( i ), QVariant::fromValue<QIcon>( QIcon( pm )), Qt::DecorationRole);
        }
    }
}


void 
SeedsWidget::onUserGetPlaylistsReturn( WsReply* r )
{
Q_UNUSED( r )
#if 0 //FIXME: No RQL for playlists yet!
    QList<WsDomElement> playlists = r->lfm().children( "playlist" );
    foreach( WsDomElement playlist, playlists )
    {
        Seed* n = new Seed( playlist[ "title" ].text(), ui.stationsBucket );

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
SeedsWidget::onItemDoubleClicked( const QModelIndex& index )
{
    if( !m_connectedAmp )
        return;
    
    SeedListView* itemView = dynamic_cast< SeedListView*>( sender() );
    Q_ASSERT( itemView );

    if( !(itemView->seedModel()->itemFromIndex( index )->flags() & Qt::ItemIsEnabled) )
        return;
    
    QStyleOptionViewItem options;
    options.initFrom( itemView );
    options.decorationSize = itemView->iconSize();
    options.displayAlignment = Qt::AlignVCenter | Qt::AlignLeft;
    options.decorationAlignment = Qt::AlignVCenter | Qt::AlignCenter;
    options.rect = itemView->visualRect( index );
    
    
    DelegateDragHint* w = new DelegateDragHint( itemView->itemDelegate( index ), index, options, itemView );
    w->setMimeData( itemView->seedModel()->mimeData( QModelIndexList() << index ) );
    w->dragToChild<QAbstractItemView*>( m_connectedAmp );
    connect( w, SIGNAL( finishedAnimation()), SLOT( onDnDAnimationFinished()));
}


void 
SeedsWidget::onAmpSeedRemoved( QString text, Seed::Type type )
{
    switch ( type ) {
        case Seed::UserType :
            foreach( Seed* item, ui.friendsBucket->seedModel()->findSeeds( text ))
            {
//TODO:                item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled );
            }
            break;
            
        case Seed::TagType:
            foreach( Seed* item, ui.tagsBucket->seedModel()->findSeeds( text ))
            {
//TODO:                item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled );
            }
            break;
            
        case Seed::PreDefinedType:
            foreach( Seed* item, ui.stationsBucket->seedModel()->findSeeds( text ))
            {
//TODO:                item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled );
            }
            break;            
        default:
            break;
    }
}


void 
SeedsWidget::connectToAmp( Amp* amp )
{
    if( m_connectedAmp )
        m_connectedAmp->disconnect( this );
    
    m_connectedAmp = amp;
    connect( m_connectedAmp, SIGNAL(itemRemoved(QString, Seed::Type)), SLOT( onAmpSeedRemoved(QString, Seed::Type)));
    connect( m_connectedAmp, SIGNAL(destroyed()), SLOT( onAmpDestroyed()));
}


void 
SeedsWidget::onContextMenuRequested( const QPoint& pos )
{
    m_cogMenu->move( pos );
    m_cogMenu->show();
}


void 
SeedsWidget::onCogMenuClicked()
{
    emit customContextMenuRequested( ui.cog->mapToGlobal( QPoint( ui.cog->geometry().width() - m_cogMenu->sizeHint().width(), ui.cog->height()) ) );
}


void 
SeedsWidget::onTabChanged()
{
    SeedListView* listView = qobject_cast< SeedListView* >( ui.tabWidget->currentWidget() );
    if( !listView )
        return;
    
    m_cogMenu->clear();
    
    foreach( QAction* a, listView->actions() )
    {
        m_cogMenu->addAction( a );
    }
       
}


void 
SeedsWidget::onTopArtistsToggled( bool b )
{
    if( !b )
        return;
    
    AuthenticatedUser authUser;
    ui.artistsBucket->seedModel()->clear();
    connect( authUser.getTopArtists(), SIGNAL(finished( WsReply* )), SLOT(onUserGetArtistsReturn( WsReply* )) );
    
}


void 
SeedsWidget::onRecentArtistsToggled( bool b )
{
    if( !b )
        return;
    
    AuthenticatedUser authUser;
    ui.artistsBucket->seedModel()->clear();
    connect( authUser.getRecentArtists(), SIGNAL(finished( WsReply* )), SLOT(onUserGetArtistsReturn( WsReply* )) );
}
/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Christian Muehlhaeuser, Last.fm Ltd <chris@last.fm>                *
 *      Max Howell, Last.fm Ltd <max@last.fm>                              *
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

#include "confirmdialog.h"
#include "container.h"
#include "UnicornCommon.h"
#include "SideBarDelegate.h"
#include "SideBarModel.h" //FIXME just for enums
#include "SideBarRevealPopup.h"
#include "SideBarToolTipLabel.h"
#include "SideBarTreeStyle.h"
#include "SideBarView.h"
#include "UglySettings.h"
#include "DragMimeData.h"
#include "lib/unicorn/QMessageBoxBuilder.h"
#include <QDropEvent>
#include <QHeaderView>
#include <QMap>
#include <QMenu>
#include <QPainter>
#include <QWidgetAction>
#include <QSlider>
#include <QIcon>
#include <QDesktopServices>
#include <QStandardItemModel>

#ifdef Q_WS_MAC
    static const int MyProfileFontSize = 12;
#else
    static const int MyProfileFontSize = 11;
#endif

namespace MooseUtils
{
    QIcon icon( QString path );
    QString dataPath( QString path );
}


SideBarTree::SideBarTree( QWidget* parent ) :
        QTreeView( parent ),
        m_timer( 0 ),
        m_empty_model( true )
{
    m_model = new SideBarModel;
    m_delegate = new SideBarDelegate;

    m_model->setParent( this );
    m_delegate->setParent( this );

    setModel( m_model );

    connect( this, SIGNAL(activated( QModelIndex )), SLOT(onActivated( QModelIndex )) );

    m_timer = new QTimer( this );
    m_timer->setSingleShot( true );
    m_timer->setInterval( 500 );
    connect( m_timer, SIGNAL(timeout()), SLOT(expandIndexUnderMouse()) );

    m_drag_tip = new ToolTipLabel( viewport() );
    m_drag_tip->setWordWrap( true );
    m_drag_tip->setAlignment( Qt::AlignCenter );
    m_drag_tip->setMargin( m_drag_tip->margin() + 3 );

  #ifndef Q_WS_X11
    // FIXME: does this really make sense on any platform?
    m_drag_tip->setFixedWidth( fontMetrics().width( 'x' ) * 30 );
  #endif

  #ifdef Q_WS_MAC
    QPalette p = palette();
    p.setColor( QPalette::Base, QColor( 0xe5, 0xed, 0xf7 ) );
    p.setColor( QPalette::Highlight, QColor( 0xa4, 0xb4, 0xcb ) );
    setPalette( p );

    setIconSize( QSize( 24, 24 ) );
    setFrameStyle( QFrame::NoFrame );

#elif defined WIN32
    if ( style()->objectName() == "windowsvista" )
    {
        setStyle( new TreeStyle<QWindowsVistaStyle> );
        setStyleSheet( "QTreeView { border: 1px solid palette(dark); border-left: 0; border-top: 0 }" );
    }
    else if ( style()->objectName() == "windowsxp" )
    {
        setStyle( new TreeStyle<QWindowsXPStyle> );
    }
    else
    {
        setStyle( new TreeStyle<QWindowsStyle> );
    }
    style()->setParent( this );

#elif defined Q_WS_X11
    if ( style()->objectName() == "cleanlooks" )
    {
        // mostly from QCleanLooksStyle.cpp
        QColor button = palette().button().color();
        QColor dark;
        dark.setHsv( button.hue(),
            qMin( 255, int(button.saturation() * 1.9) ),
            qMin( 255, int(button.value() * 0.7) ) );
        dark = dark.light( 108 );

        QString sheet = "QTreeView { border: 1px solid rgb( %1, %2, %3 ); border-top: 0 }";
        sheet = sheet.arg( dark.red() ).arg( dark.green() ).arg( dark.blue() );

        setStyleSheet( sheet + "QTreeView::branch:adjoins-item { border-color: none }" );
    }
    else
    {
        setStyle( new TreeStyle<QPlastiqueStyle> );
        setStyleSheet( "QTreeView { border: 0px }" );
    }

#endif

    header()->hide();

    setItemDelegate( m_delegate );
    setRootIsDecorated( true );
    setDragEnabled( true );
    setAutoScroll( true );
    setAcceptDrops( true );
    setDropIndicatorShown( false );
    setSelectionMode( QAbstractItemView::SingleSelection );
    setSelectionBehavior( QAbstractItemView::SelectRows );
    setEditTriggers( QAbstractItemView::NoEditTriggers );
    setDragDropMode( DragDrop );
    setMouseTracking( true );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

  #if QT_VERSION >= 0x04030000
    setAttribute( Qt::WA_MacShowFocusRect, false );
  #endif

//TODO    connect( &The::container(), SIGNAL(stackIndexChanged( int )), SLOT(onContainerPageChanged( int )) );
	connect( qApp, SIGNAL(event( int, QVariant )), SLOT(onAppEvent( int, QVariant )) );
}


#if 0
void
SideBarTree::onAppEvent( int e, const QVariant& )
{
    if (e == Event::UserChanged)
    {
        if (The::user().settings().sidebarEnabled())
            makeWebRequests();
        else {
            // model will empty itself via UserChanged signal, this should be safe
            // it will get its signal straight away
            m_empty_model = true; 
        }
    }
}
#endif


void
SideBarTree::showEvent( QShowEvent* )
{
    // we only populate the model if the sidebar is visible
    // rationale: save Last.fm's bandwidth
    
    if (m_empty_model)
        makeWebRequests();
}


void
SideBarTree::makeWebRequests()
{
    m_empty_model = false;

    //TODO
#if 0    
    (new FriendsRequest)->start();
    (new NeighboursRequest)->start();
    (new UserTagsRequest)->start();
    (new RecentTracksRequest)->start();
    (new RecentlyLovedTracksRequest)->start();

    UserPicturesRequest* picReq = new UserPicturesRequest();
    picReq->setNames( QStringList() << The::user().name() );
    picReq->setImageSize( SIZE_MEDIUM );
    picReq->start();

    (new RecentlyBannedTracksRequest)->start();
#endif
}

void
SideBarTree::addRecentlyPlayedTrack( Track track )
{
    m_model->addRecentlyPlayedTrack( track );
}


#ifdef Q_WS_MAC
void
SideBarTree::drawBranches( QPainter* painter, const QRect& rect, const QModelIndex& index ) const
{
    QRect r = rect;
    r.translate( 7, 0 ); //move the branch root arrows right 7 px on mac
    QTreeView::drawBranches( painter, r, index );
}
#endif


void
SideBarTree::drawRow( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& i ) const
{
    //TODO silly! REMOVE
    m_delegate->setFont( font() );
    m_delegate->setSideBarWidth( size().width() );

    if (!i.parent().isValid())
        switch (i.row())
        {
            case SideBar::MyProfile:
            {
                QFont font = painter->font();
                font.setPixelSize( MyProfileFontSize );
                painter->setFont( font );

                painter->setViewTransformEnabled( true );

                QRect rect = option.rect;

                #ifdef LINUX
                if (qApp->style()->objectName() == "cleanlooks")
                    rect.adjust( 2, 1, width() - 2, -5 );
                else
                    rect.adjust( 0, 0, width() - 2, -6 );

                painter->setPen( QColor( 0xab, 0xb9, 0xcf ).light( 125 ) );
                painter->drawRect( rect.adjusted( -1, -1, 0, -1 ) );

                #else
                rect.setWidth( width() );
                #endif

                painter->setPen( Qt::white );
                QLinearGradient gradient( 0, 0, 0, rect.height() );
                gradient.setColorAt( 0, QColor( 0xab, 0xb9, 0xcf ) );
                gradient.setColorAt( 1, QColor( 0x99, 0xab, 0xc4 ) );
                painter->fillRect( rect, QBrush( gradient ) );

                #ifndef WIN32
                painter->setPen( QColor( 0xab, 0xb9, 0xcf ).dark( 120 ) );
                painter->drawLine( rect.bottomLeft(), rect.bottomRight() );
                #endif

                QPixmap avatar = i.data( Qt::DecorationRole ).value<QPixmap>();
                if (!avatar.isNull()) {
                    #ifdef LINUX
                    QPoint p = rect.topLeft();
                    QColor c = QColor( 0xab, 0xb9, 0xcf ).light( 103 );
                    painter->setPen( c );
                    painter->drawLine( p + QPoint( avatar.width(), 0 ), p + QPoint( avatar.width(), avatar.height() - 1 ) );
                    #else
                    QPoint p( 12 + (30 - avatar.width()) / 2, (rect.height() - 30) / 2 );
                    #endif
                    painter->drawPixmap( p, avatar );
                }

                QTextOption noWrap;
                noWrap.setWrapMode( QTextOption::NoWrap );
              #ifdef Q_WS_MAC
                rect.adjust( 48, 10, 0, -10 );
              #elif defined LINUX
                rect.adjust( 44, 6, 0, -6 );
              #else
                rect.adjust( 48, 6, 0, -6 );
              #endif

                QFont f = painter->font();
                f.setWeight( QFont::Light );
                painter->setPen( Qt::white );
                painter->drawText( rect, m_delegate->truncateString( tr( "My Music Profile" ), f, size().width() ), noWrap );

                rect.adjust( 0, painter->fontMetrics().height() - 2, 0, 0 );

                painter->drawText( rect, m_delegate->truncateString( i.data().toString(), painter->font(), size().width() ), noWrap );

                break;
            }

            case SideBar::StartAStation:
            {
                QStyleOptionViewItem o = option;
//TODO                if ( The::container().stackIndex() == 0 )
//TODO                    o.state |= QStyle::State_Selected | QStyle::State_Active;

                QTreeView::drawRow( painter, o, i );
                break;
            }

            case SideBar::NowPlaying:
            {
                QStyleOptionViewItem o = option;
//TODO                if ( The::container().stackIndex() == 1 )
//TODO                    o.state |= QStyle::State_Selected | QStyle::State_Active;

                QTreeView::drawRow( painter, o, i );
                break;
            }

            default:
                QTreeView::drawRow( painter, option, i );
                break;
        }
    else
        QTreeView::drawRow( painter, option, i );
}


void
SideBarTree::contextMenuEvent( QContextMenuEvent* e )
{
    // We don't want right click interaction with the MyProfile element
    if ( SideBarItem( indexAt( e->pos() ) ).type() != SideBar::MyProfile )
        contextMenuHandler( indexAt( e->pos() ), ExecQMenu );
}


void
SideBarTree::onContainerPageChanged( int index )
{
    //in case the current index is the one we don't want to render selected
    setCurrentIndex( model()->index( SideBar::StartAStation + index, 0 ) );

    QModelIndex i = model()->index( SideBar::StartAStation, 0 );
    QModelIndex j = model()->index( SideBar::NowPlaying, 0 );

    dataChanged( i, j );
}


void
SideBarTree::currentChanged( const QModelIndex& current, const QModelIndex& /* previous */ )
{
    if ( current.parent().isValid() )
        return;

    switch ( current.row() )
    {
        case SideBar::NowPlaying:
        {
            emit plsShowNowPlaying();
        }
        break;

        case SideBar::StartAStation:
        {
            emit plsShowRestState();
        }
        break;

        default:
            return;
    }
}


void
SideBarTree::onActivated( const QModelIndex& i )
{
    contextMenuHandler( i, DoQMenuDefaultAction );
}


void
SideBarTree::contextMenuHandler( const QModelIndex& index, ContextMenuActionType the_action_to_do )
{
    //TODO Woah! This became huge. DRY!
    //REFACTOR IMMEDIATELY!
    // No, refactor when we have the need to edit this code again. ;)

    if (!index.isValid())
        return;

    enum ActionType { PlayStation,                  PlayGlobalTagRadio,
                      PlayUserTagRadio,             PlayPersonalRadio,
                      PlayNeighbourRadio,           PlayLovedRadio,
                      DeleteFriend,                 UnLove,
                      Love,                         UnBan,
                      Ban,                          ClearHistory,
                      PlayTrack,                    TagTrack,
                      RecommendTrack,               ChangeHistorySize,
                      RemoveHistoryStation,         SortTagsByPopularity,
                      SortTagsAZ,                   SortTagsZA,
                      SortNeighboursBySimilarity,   SortNeighboursAZ,
                      SortNeighboursZA,             GoToTrackPage,
                      GoToTagPage,                  GoToUserPage,
                      GoToFriendsPage,              GoToNeighboursPage,
                      GoToTagsPage,                 AddToPlaylist,
                      NoAction };

    QString const localizedHost = UnicornUtils::localizedHostName( The::settings().appLanguage() );
    QString text = index.data().toString().replace( '&', "&&" );
    QString const PLAY_THIS_STATION = tr("&Play This Station");
    SideBarItem i( index );
    QMenu menu( this );
    menu.setSeparatorsCollapsible( true );
    QMap<int, QAction*> map;
    map[NoAction] = 0;

    #ifdef Q_WS_MAC
    // Mac GUI guidelines wants ellipsis after actions that always pop up confirm dialogs.
    QString const ellipsis = tr( "..." );
    #else
    QString const ellipsis;
    #endif

    switch (i.type())
    {
        case SideBar::MyProfile:
            QDesktopServices::openUrl( "http://" + localizedHost + "/user/" + QUrl::toPercentEncoding( The::settings().currentUsername() ) );
            break;
        case SideBar::HistoryStation:
            map[PlayStation] = menu.addAction( i.icon(), PLAY_THIS_STATION );
            menu.setDefaultAction( map[PlayStation] );
            menu.addSeparator();
            map[RemoveHistoryStation] = menu.addAction( tr("&Remove From History") );
            break;

        case SideBar::MyRecommendations:
        case SideBar::PersonalRadio:
        case SideBar::LovedTracksRadio:
        case SideBar::NeighbourhoodRadio:
            map[PlayStation] = menu.addAction( i.icon(), PLAY_THIS_STATION );
            menu.setDefaultAction( map[PlayStation] );
            break;

        case SideBar::MyTags:
        {
            map[GoToTagsPage] = menu.addAction( i.icon(), tr("Go To My Tags Page") );
            menu.addSeparator();
            map[SortTagsByPopularity] = menu.addAction( tr("Sort By Popularity") );
            map[SortTagsByPopularity]->setCheckable( true );
            map[SortTagsAZ] = menu.addAction( tr("Sort A-Z") );
            map[SortTagsAZ]->setCheckable( true );
            map[SortTagsZA] = menu.addAction( tr("Sort Z-A") );
            map[SortTagsZA]->setCheckable( true );

            QActionGroup* sortActions = new QActionGroup( this );
            sortActions->addAction( map[SortTagsByPopularity] );
            sortActions->addAction( map[SortTagsAZ] );
            sortActions->addAction( map[SortTagsZA] );

            sortActions->actions().at( The::settings().currentUser().sideBarTagsSortOrder() )->setChecked( true );
        }
        break;

        case SideBar::MyTagsChild:
        {
            text = text.remove( text.lastIndexOf( " (" ), text.length() );
            map[PlayStation] = menu.addAction( i.icon(), tr("Play This Tag Station") );
            map[PlayUserTagRadio] = menu.addAction( i.icon(), tr("Play Only Music You Tagged \"%1\"").arg( text ) );
            menu.addSeparator();
            map[GoToTagPage] = menu.addAction( tr("Go To Tag Page") );
            menu.addSeparator();
            map[SortTagsByPopularity] = menu.addAction( tr("Sort Tags By Popularity") );
            map[SortTagsByPopularity]->setCheckable( true );
            map[SortTagsAZ] = menu.addAction( tr("Sort A-Z") );
            map[SortTagsAZ]->setCheckable( true );
            map[SortTagsZA] = menu.addAction( tr("Sort Z-A") );
            map[SortTagsZA]->setCheckable( true );

            QActionGroup* sortActions = new QActionGroup( this );
            sortActions->addAction( map[SortTagsByPopularity] );
            sortActions->addAction( map[SortTagsAZ] );
            sortActions->addAction( map[SortTagsZA] );

            sortActions->actions().at( The::settings().currentUser().sideBarTagsSortOrder() )->setChecked( true );

//TODO            if ( The::user().isSubscriber() )
//TODO                menu.setDefaultAction( map[PlayUserTagRadio] );
//TODO            else
                menu.setDefaultAction( map[PlayStation] );
        }
        break;

        case SideBar::Friends:
            map[GoToFriendsPage] = menu.addAction( tr("Go To My Friends Page") );
        break;

        case SideBar::Neighbours:
        {
            map[GoToNeighboursPage] = menu.addAction( tr("Go To My Neighbours Page") );
            menu.addSeparator();
            map[SortNeighboursBySimilarity] = menu.addAction( tr("Sort By Similarity") );
            map[SortNeighboursBySimilarity]->setCheckable( true );
            map[SortNeighboursAZ] = menu.addAction( tr("Sort A-Z") );
            map[SortNeighboursAZ]->setCheckable( true );
            map[SortNeighboursZA] = menu.addAction( tr("Sort Z-A") );
            map[SortNeighboursZA]->setCheckable( true );

            QActionGroup* sortActions = new QActionGroup( this );
            sortActions->addAction( map[SortNeighboursBySimilarity] );
            sortActions->addAction( map[SortNeighboursAZ] );
            sortActions->addAction( map[SortNeighboursZA] );

            sortActions->actions().at( The::settings().currentUser().sideBarNeighbourSortOrder() )->setChecked( true );
        }
        break;

        case SideBar::FriendsChild:
        case SideBar::NeighboursChild:
        {
            map[PlayPersonalRadio] = menu.addAction( MooseUtils::icon( "personal_radio" ), tr( "Play %1's Radio Station" ).arg( text ) );
            map[PlayNeighbourRadio] = menu.addAction( MooseUtils::icon( "neighbour_radio" ), tr( "Play %1's Neighbourhood" ).arg( text ) );
            map[PlayLovedRadio] = menu.addAction( MooseUtils::icon( "loved_radio" ), tr( "Play %1's Loved Tracks" ).arg( text ) );
            menu.addSeparator();
            map[GoToUserPage] = menu.addAction( tr( "Go To %1's Profile" ).arg( text ) );


            if (i.type() == SideBar::FriendsChild)
            {
                menu.addSeparator();
                map[DeleteFriend] = menu.addAction( tr( "End Friendship" ) + ellipsis );
            }
            else if (i.type() == SideBar::NeighboursChild)
            {
                menu.addSeparator();
                map[SortNeighboursBySimilarity] = menu.addAction( tr( "Sort By Similarity" ) );
                map[SortNeighboursBySimilarity]->setCheckable( true );
                map[SortNeighboursAZ] = menu.addAction( tr( "Sort A-Z" ) );
                map[SortNeighboursAZ]->setCheckable( true );
                map[SortNeighboursZA] = menu.addAction( tr( "Sort Z-A" ) );
                map[SortNeighboursZA]->setCheckable( true );

                QActionGroup* sortActions = new QActionGroup( this );
                sortActions->addAction( map[SortNeighboursBySimilarity] );
                sortActions->addAction( map[SortNeighboursAZ] );
                sortActions->addAction( map[SortNeighboursZA] );

                sortActions->actions().at( The::settings().currentUser().sideBarNeighbourSortOrder() )->setChecked( true );
            }

            menu.setDefaultAction( map[PlayPersonalRadio] );
            break;
        }

        case SideBar::RecentlyPlayedTrack:
            map[PlayTrack] = menu.addAction( tr( "Play Track" ) );
            menu.addSeparator();
            map[Love] = menu.addAction( QIcon( MooseUtils::dataPath( "buttons/love.png" ) ), tr( "Add To My Loved Tracks" ) + ellipsis );
            #ifndef HIDE_RADIO
            map[Ban] = menu.addAction( QIcon( MooseUtils::dataPath( "buttons/ban.png" ) ), tr( "Add To My Banned Tracks" ) + ellipsis );
            map[AddToPlaylist] = menu.addAction( QIcon( ":/action/Playlist.png" ), tr( "Add To My Playlist" ) + ellipsis );
            #endif
            menu.addSeparator();
            map[RecommendTrack] = menu.addAction( QIcon( MooseUtils::dataPath( "buttons/recommend.png" ) ), tr( "Share..." ) );
            map[TagTrack] = menu.addAction( QIcon( MooseUtils::dataPath( "buttons/tag.png" ) ), tr( "Tag..." ) );
            menu.addSeparator();
            map[GoToTrackPage] = menu.addAction( tr( "Go To Track Page" ) );

            menu.setDefaultAction( map[PlayTrack] );
            break;

        case SideBar::RecentlyLovedTrack:
            map[PlayTrack] = menu.addAction( tr( "Play Track" ) );
            menu.addSeparator();
            #ifndef HIDE_RADIO
            map[AddToPlaylist] = menu.addAction( QIcon( ":/action/Playlist.png" ), tr( "Add To My Playlist" ) + ellipsis );
            menu.addSeparator();
            #endif
            map[RecommendTrack] = menu.addAction( QIcon( MooseUtils::dataPath( "buttons/recommend.png" ) ), tr( "Share..." ) );
            map[TagTrack] = menu.addAction( QIcon( MooseUtils::dataPath( "buttons/tag.png" ) ), tr( "Tag..." ) );
            menu.addSeparator();
            map[GoToTrackPage] = menu.addAction( tr( "Go To Track Page" ) );
            menu.addSeparator();
            map[UnLove] = menu.addAction( tr( "Remove From My Loved Tracks" ) + ellipsis );

            menu.setDefaultAction( map[PlayTrack] );
            break;

        case SideBar::RecentlyBannedTrack:
            map[PlayTrack] = menu.addAction( tr( "Play Track" ) );
            menu.addSeparator();
            map[RecommendTrack] = menu.addAction( QIcon( MooseUtils::dataPath( "buttons/recommend.png" ) ), tr( "Share..." ) );
            map[TagTrack] = menu.addAction( QIcon( MooseUtils::dataPath( "buttons/tag.png" ) ), tr( "Tag..." ) );
            menu.addSeparator();
            map[GoToTrackPage] = menu.addAction( tr( "Go To Track Page" ) );
            #ifndef HIDE_RADIO
            menu.addSeparator();
            map[UnBan] = menu.addAction( tr( "Un-ban This Track" ) + ellipsis );
            #endif

            menu.setDefaultAction( map[PlayTrack] );
            break;

        case SideBar::History:
            map[ClearHistory] = menu.addAction( tr( "&Empty Station History" ) );
            break;

        default:
            break;
    }

  #ifdef Q_WS_MAC
    // Hannah says that Mac context menus have no icons
    foreach (QAction *a, menu.actions())
        a->setIcon( QIcon() );
  #endif

  #ifdef HIDE_RADIO
    delete map[PlayPersonalRadio];
    delete map[PlayNeighbourRadio];
    delete map[PlayLovedRadio];
    delete map[PlayStation];
    delete map[PlayUserTagRadio];
    delete map[PlayTrack];
  #endif

    QAction *a = 0;
    if ( the_action_to_do == ExecQMenu && menu.actions().count() )
        a = menu.exec( QCursor::pos() );
    else
        a = menu.defaultAction();

    QString const display_role = index.data().toString();
    QString const encoded_display_role = QUrl::toPercentEncoding( display_role );

    switch (map.key( a ))
    {
        case RemoveHistoryStation:
            The::currentUser().removeRecentStation( index.row() );
            break;

        case ClearHistory:
            The::currentUser().clearRecentStations( true );
            break;

        //FIXME this is all stupid, put this data in the model that's what it is for! --mxcl

        case PlayPersonalRadio:
//TODO            The::radio().playStation(
//TODO                StationUrl( "lastfm://user/" + encoded_display_role + "/personal" ) );
            break;

        case PlayNeighbourRadio:
//TODO            The::radio().playStation(
//TODO                StationUrl( "lastfm://user/" + encoded_display_role + "/neighbours" ) );
            break;

        case PlayLovedRadio:
//TODO            The::radio().playStation(
//TODO                StationUrl( "lastfm://user/" + encoded_display_role + "/loved" ) );
            break;

        case PlayUserTagRadio:
        {
            QString const tag = index.data().toString().remove( QRegExp(" \\(\\d*\\)$") );
//TODO            The::radio().playStation( StationUrl( "lastfm://usertags/" +
//TODO                    QUrl::toPercentEncoding( The::currentUsername() ) + '/' +
//TODO                    tag ) );
            break;
        }

        case PlayStation:
//TODO            The::radio().playStation( StationUrl( index.data( SideBar::StationUrlRole ).toString() ) );
            break;

        case SortTagsByPopularity:
            m_model->sortTags( SideBar::MostWeightOrder );
            The::settings().currentUser().setSideBarTagsSortOrder(0);
            break;

        case SortTagsAZ:
            m_model->sortTags( SideBar::AscendingOrder );
            The::settings().currentUser().setSideBarTagsSortOrder(1);
            break;

        case SortTagsZA:
            m_model->sortTags( SideBar::DescendingOrder );
            The::currentUser().setSideBarTagsSortOrder(2);
            break;

        case SortNeighboursBySimilarity:
            m_model->sortNeighbours( SideBar::MostWeightOrder );
            The::currentUser().setSideBarNeighbourSortOrder(0);
            break;

        case SortNeighboursAZ:
            m_model->sortNeighbours( SideBar::AscendingOrder );
            The::currentUser().setSideBarNeighbourSortOrder(1);
            break;

        case SortNeighboursZA:
            m_model->sortNeighbours( SideBar::DescendingOrder );
            The::currentUser().setSideBarNeighbourSortOrder(2);
            break;

        case GoToTagPage:
        {
            QString const tag = index.data().toString().remove( QRegExp(" \\(\\d*\\)$") );

            QDesktopServices::openUrl( "http://" + localizedHost + "/tag/" +
                                       UnicornUtils::urlEncodeItem( tag ) );
            break;
        }

        case GoToUserPage:
            QDesktopServices::openUrl( "http://" + localizedHost + "/user/" +
                                       UnicornUtils::urlEncodeItem( display_role ) );
            break;

        case GoToFriendsPage:
            QDesktopServices::openUrl( "http://" + localizedHost + "/user/" +
                                       UnicornUtils::urlEncodeItem( The::currentUsername() ) + "/friends/" );
            break;

        case GoToNeighboursPage:
            QDesktopServices::openUrl( "http://" + localizedHost + "/user/" +
                                       UnicornUtils::urlEncodeItem( The::currentUsername() ) + "/neighbours/" );
            break;

        case GoToTagsPage:
        {
            QDesktopServices::openUrl( "http://" + localizedHost + "/user/" +
                                       UnicornUtils::urlEncodeItem( The::currentUsername() ) + "/tags/" );
            break;
        }

        case GoToTrackPage:
            QDesktopServices::openUrl( "http://" + localizedHost + "/music/" +
                                       UnicornUtils::urlEncodeItem( i.track().artist() ) + "/_/" +
                                       UnicornUtils::urlEncodeItem( i.track().title() ) + "/" );
            break;

        case PlayTrack:
//TODO            The::radio().play( i.track() );
            break;

        case RecommendTrack:
        {
//TODO            The::shareDialog().setSong( i.track() );
//TODO            The::shareDialog().exec();
        }
        break;

        case TagTrack:
//TODO            TagDialog( i.track(), this ).exec();
            break;

        #define barg( text ) \
            arg( "\"" + text + "\"" )

        #define confirm( text ) \
                QMessageBoxBuilder( this ) \
                    .setButtons( QMessageBox::Yes | QMessageBox::No ) \
                    .setTitle( tr("Confirm") ) \
                    .setText( text ).exec() == QMessageBox::Yes

        case DeleteFriend:
//TODO            if (confirm( tr("Do you really want to remove %1 from your friends list?").barg( index.data().toString() ) ))
//TODO                (new DeleteFriendRequest( index.data().toString() ))->start();
            break;

        case Love:
//TODO            if (ConfirmDialog::love( i.track(), this ))
//TODO          (new LoveRequest( i.track() ))->start();
            break;

        case UnLove:
//TODO            if (confirm( tr("Do you really want to remove %1 from your Loved Tracks?").barg( index.data().toString() ) ))
//TODO                (new UnLoveRequest( i.track() ))->start();
            break;

        case Ban:
//TODO            if (ConfirmDialog::ban( i.track(), this ))
//TODO                (new BanRequest( i.track() ))->start();
            break;

        case UnBan:
//TODO            if (confirm( tr( "Do you really want to un-ban %1?" ).arg( index.data().toString() ) ))
//TODO                (new UnBanRequest( i.track() ))->start();
            break;

        case AddToPlaylist:
//TODO            if (ConfirmDialog::playlist( i.track(), this ))
//TODO                (new AddToMyPlaylistRequest( i.track() ))->start();
            break;

        #undef confirm

        case NoAction:
            break;
    }

    // see macro above, we need to remove from macro to get translated I think
    (void) tr("Confirm");
}


void
SideBarTree::dragLeaveEvent( QDragLeaveEvent* e )
{
    QTreeView::dragLeaveEvent( e );
    m_timer->stop();
}


void
SideBarTree::dragMoveEvent( QDragMoveEvent* e )
{
    // Call parent one first, otherwise we'd accept all D&D events
    QTreeView::dragMoveEvent( e );
    dragDropHandler( e );
}


void
SideBarTree::dropEvent( QDropEvent* e )
{
//     QTreeView::dropEvent( e );
    dragDropHandler( e );
}


void
SideBarTree::dragDropHandler( QDropEvent* e )
{
    QModelIndex i = indexAt( e->pos() );
    if (!i.isValid())
        return; //TODO throw exception! Why?

    QString message;

    bool const you_should_accept = dragDropHandlerPrivate( i, e, message );
    QDragMoveEvent *dmev = dynamic_cast<QDragMoveEvent*>(e);
    bool const is_drag = dmev;

    if (is_drag) {
        if (you_should_accept)
            dmev->accept( visualRect( i ) );
        else
            dmev->ignore( visualRect( i ) );
    }
    else
        e->setAccepted( you_should_accept );

    if (!is_drag) {
        emit statusMessage( message );
        m_drag_tip->hide();
    }
    else if (!message.isEmpty())
    {
        #ifndef Q_WS_X11
        m_drag_tip->setText( message );
        m_drag_tip->adjustSize(); //FIXME Qt sucks, often height is too small

        QRect r = visualRect( i );

        QPoint p( m_drag_tip->pos() );
        p.ry()  = viewport()->mapToGlobal( r.topRight() ).y();
        p.ry() += r.height() / 2;
        p.ry() -= m_drag_tip->height() / 2;

        // don't jerk about left and right as that is unpolished
        if (!m_drag_tip->isVisible())
        {
            p.rx() = mapToGlobal( geometry().topRight() ).x() + 10;
        }

        m_drag_tip->move( p );
        m_drag_tip->show();
        #else
            // I can't figure out why, but m_drag_tip->show() breaks the event
            // handling on linux. You have to re-click to actually make the drop
            // really happen. Since it works on Win/Mac we'll use this as a work-
            // around in the meantime.
            emit statusMessage( message );
        #endif
    }
    else
        m_drag_tip->hide();
}


/**
 * @return whether you should accept the event, if this is a drag handler
 */

bool
SideBarTree::dragDropHandlerPrivate( const QModelIndex& i, QDropEvent* event, QString &status )
{
    // we handle drag and drop in the same place to ensure that we don't ie
    // reject the drag, when actually we can do a drop, splitting the code would
    // encourage that kind of bug. Shame this turned into a mahusive function
    // as a result :( --mxcl

    setCurrentIndex( i );

    bool const is_drop = dynamic_cast<QDragMoveEvent*>(event) == 0;
    if (is_drop)
        m_timer->stop();
    else
        m_timer->start();

    DragMimeData const *mimedata = static_cast<const DragMimeData*>(event->mimeData());
    SideBarItem droptarget( i );

//////
    switch (mimedata->itemType())
    {
        case UnicornEnums::ItemArtist:
        case UnicornEnums::ItemAlbum:
        case UnicornEnums::ItemTrack:
            switch (droptarget.type())
            {
                case SideBar::FriendsChild:
                case SideBar::NeighboursChild:
                {
                    TrackInfo const track = mimedata->track();
                    QString const username = i.data().toString();

                    status = tr("Shares %1 with %2").barg( track.toString() ).barg( username );
                    if (is_drop && ConfirmDialog::share( track, username, this ))
                    {
//TODO                        RecommendRequest* r = new RecommendRequest( mimedata, username );
//TODO                        r->setLanguage( The::settings().appLanguage() );
//TODO                        r->start();
                    }
                    return true;
                }

                case SideBar::MyTagsChild:
                {
                    TrackInfo const track = mimedata->track();
                    QString const tag = i.data().toString().remove( QRegExp(" \\(\\d*\\)$") );

                    QString trackData;

                    switch( mimedata->itemType() )
                    {
                        case UnicornEnums::ItemArtist:
                            trackData = track.artist();
                            break;

                        case UnicornEnums::ItemAlbum:
                            trackData = track.album();
                            break;

                        case UnicornEnums::ItemTrack:
                            trackData = track.toString();
                            break;

                        default:
                            break;
                    }

                    status = tr("Tags %1 as %2").barg( trackData ).barg( tag );

                    if (is_drop && ConfirmDialog::tag( trackData, tag, this ))
                    {
//TODO                        SetTagRequest::append( mimedata, tag );
                    }
                    return true;
                }
                default:
                    break;
            }

        default:
            break;
    }

    if (mimedata->hasTrack())
    {
        Track track = mimedata->track();

        switch (droptarget.type())
        {
            case SideBar::RecentlyPlayed:
                status = tr("You can't add tracks to Recently Played");
                return false;

            case SideBar::RecentlyLoved:
            case SideBar::LovedTracksRadio:
                status = tr("Loves %1").barg( track.toString() );
//TODO                if (is_drop && ConfirmDialog::love( track, this ))
//TODO                    (new LoveRequest( track ))->start();
                return true;

            case SideBar::RecentlyBanned:
                status = tr("Bans %1").barg( track.toString() );
                if (is_drop && ConfirmDialog::ban( track, this ))
                {
//TODO                    (new BanRequest( track ))->start();
                }
                return true;

            case SideBar::MyProfile:
            case SideBar::MyRecommendations:
                status = tr("Shares %1 with yourself").barg( track.toString() );
                if (is_drop)
                {
//TODO                    RecommendRequest* r = new RecommendRequest( track, i.data().toString() );
//TODO                    r->setLanguage( The::settings().appLanguage() );
//TODO                    r->start();
                }
                return true;

            default:
                return false;
        }
    }

    if (mimedata->hasUser())
    {
        if (droptarget.classification() == SideBarItem::Track)
        {
            Track const track = droptarget.track();
            QString const username = mimedata->username();

            status = tr("Shares %1 with %2").barg( track.toString() ).barg( username );

            if (is_drop && ConfirmDialog::share( track, username, this ))
            {
//TODO                RecommendRequest* r = new RecommendRequest( track, username );
//TODO                r->setLanguage( The::settings().appLanguage() );
//TODO                r->start();
            }
            return true;
        }
        else
            return false;
    }

    if (mimedata->hasTag())
    {
        if (droptarget.classification() == SideBarItem::Track)
        {
            Track const track = droptarget.track();
            QString const tag = mimedata->tag();

            status = tr("Tag %1 as %2").barg( track.toString() ).barg( tag );

//TODO            if (is_drop && ConfirmDialog::tag( track, tag, this ))
            {
//TODO                SetTagRequest::append( track, tag );
            }
            return true;
        }
        else
            return false;
    }

    return true;
}


void
SideBarTree::expandIndexUnderMouse()
{
    QModelIndex i = indexAt( mapFromGlobal( QCursor::pos() ) );

    if (i.isValid() && i.model()->hasChildren( i ))
        expand( i );
}


void
SideBarTree::mouseMoveEvent( QMouseEvent* e )
{
    return QTreeView::mouseMoveEvent( e );

    // we do our own tooltip as it gives us more control, and Qt does very bad tooltips

    QModelIndex i = indexAt( e->pos() );

    if (i.parent().isValid())
    {
        int const needed_width = itemDelegate()->sizeHint( viewOptions(), i ).width();
        QRect index_rect = visualRect( i );

        if (!m_revealer)
            m_revealer = new RevealPopup( viewport() );

        if (m_revealer->isVisible() || needed_width > viewport()->width() - index_rect.left())
        {
        #ifdef Q_WS_MAC
            int const w = 6;
            int const h = 1;
        #else
            int const w = 22;
            int const h = -3;
        #endif

            QPoint p = index_rect.topLeft() + QPoint( w, h );
            p = viewport()->mapToGlobal( p );

            QString text =  i.data().toString();
            if (e->modifiers() & Qt::AltModifier) {
                QString s = i.data( Qt::ToolTipRole ).toString();
                if (!s.isEmpty())
                    text = s;
            }
            else if (p == m_revealer->pos() && m_revealer->isVisible())
                return;

            m_revealer->setFont( i.data( Qt::FontRole ).value<QFont>() );
            m_revealer->setText( text );
            m_revealer->adjustSize();
            m_revealer->move( p );
            m_revealer->show();
        }
        else
            m_revealer->hide();
    }
    else if (m_revealer)
        m_revealer->hide();

    QTreeView::mouseMoveEvent( e );
}


void
SideBarTree::mousePressEvent( QMouseEvent* _e )
{
    #ifdef Q_WS_MAC
    // we have to shift the mouse event left 7 pixels as we render everything 7
    // pixels to the right on mac
    QMouseEvent E( _e->type(), _e->pos() - QPoint( 7, 0 ), _e->button(), _e->buttons(), _e->modifiers() );
    QMouseEvent *e = &E;
    #else
    QMouseEvent *e = _e;
    #endif

    //HACK unfortunately necessary to prevent this index becoming current
    //NOTE doesn't prevent keyboard selection, but screw it
    QModelIndex i = indexAt( viewport()->mapFromGlobal( e->globalPos() ) );
    if (!i.parent().isValid() && i.row() == SideBar::NowPlaying)
        if ((model()->flags( i ) & Qt::ItemIsEnabled) == 0)
            return;

    QTreeView::mousePressEvent( e );
}


void
SideBarTree::scrollContentsBy( int /* dx */, int dy )
{
    // Disallow horizontal scrolling
    QTreeView::scrollContentsBy( 0, dy );
}


#if 0
// part of the show tooltip over the top of text code that was disabled due to bugs
bool
SideBarTree::viewportEvent( QEvent* e )
{
    // if we are showing a revealer tip, change it to the tooltip text on
    // tooltip events as this is neater.

    if (e->type() == QEvent::ToolTip && m_revealer && m_revealer->isVisible())
    {
        QMouseEvent e( QEvent::MouseMove,
                    mapFromGlobal( QCursor::pos() ),
                    Qt::NoButton,
                    Qt::NoButton,
                    Qt::AltModifier // holding alt shows the tooltip
                    );

        mouseMoveEvent( &e );
        return true;
    }
}
#endif


#ifdef HIDE_RADIO
void
SideBarTree::reset()
{
    QTreeView::reset();

    using namespace SideBar;
    
    QModelIndex blank;
    setRowHidden( StartAStation, blank, true );
    setRowHidden( MyRecommendations, blank, true );
    setRowHidden( PersonalRadio, blank, true );
    setRowHidden( LovedTracksRadio, blank, true );
    setRowHidden( NeighbourhoodRadio, blank, true );
    setRowHidden( Spacer2, blank, true );
    setRowHidden( RecentlyBanned, blank, true );
    setRowHidden( History, blank, true );
}
#endif

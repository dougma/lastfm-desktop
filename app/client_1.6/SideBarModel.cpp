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

#include "container.h"
#include "UglySettings.h"
#include "SideBarModel.h"
#include <QPainter>
#include <QMimeData>
#define USER_AVATAR_HOST "userserve-ak.last.fm"

using namespace SideBar;

namespace MooseUtils
{
    QIcon icon( QString path ) { return QIcon( ":/icons/" + path ); }
}


SideBarModel::SideBarModel()
        : m_track_is_playing( false )
{
    m_change_station_text = tr("Start a Station");
}


void
SideBarModel::addRecentlyPlayedTrack( Track track )
{
    QList<Track> played = m_played;
    played.prepend( track );

    changeData( SideBar::RecentlyPlayed, m_played, played );
}


QVariant
SideBarModel::data( const QModelIndex &index, int role ) const
{
    if (!index.isValid())
        return QVariant();

    SideBarItem i( index );

    if (role == Qt::DisplayRole)
        switch (i.type()) {
            case MyProfile:           return The::currentUsername();
            case StartAStation:       return m_change_station_text;
            case NowPlaying:          return tr("Now Playing");
            case MyRecommendations:   return tr("My Recommendations");
            case PersonalRadio:       return tr("My Radio Station");
            case LovedTracksRadio:    return tr("My Loved Tracks");
            case NeighbourhoodRadio:  return tr("My Neighbourhood");
            case RecentlyPlayed:      return tr("Recently Played");
            case RecentlyLoved:       return tr("Recently Loved");
            case RecentlyBanned:      return tr("Recently Banned");
            case MyTags:              return tr("My Tags");
            case Friends:             return tr("Friends");
            case Neighbours:          return tr("Neighbours");
            case History:             return tr("History");

            case RecentlyPlayedTrack: return m_played.value( index.row() );
            case RecentlyLovedTrack:  return m_loved.value( index.row() );
            case RecentlyBannedTrack: return m_banned.value( index.row() );
            case MyTagsChild:         return m_tags.value( index.row() );
            case FriendsChild:        return m_friends.value( index.row() );
            case NeighboursChild:     return m_neighbours.value( index.row() );
            case HistoryStation:      return m_history.value( index.row() ).title();

            default: break; //gcc warning--
        }

    if (role == Qt::ToolTipRole)
        switch ((int)i.type()) {
            case RecentlyPlayedTrack:
            case RecentlyLovedTrack:
            case RecentlyBannedTrack:
            case MyTagsChild:
            case FriendsChild:
            case NeighboursChild:
            case HistoryStation:
                return data( index, Qt::DisplayRole );
                //return data( index, UrlRole );
        }

    if (role == Qt::DecorationRole)
        switch (i.type()) {
            case MyProfile:           return m_avatar;
            case StartAStation:       return MooseUtils::icon( "icon_radio" );
            case NowPlaying:          return QIcon( ":/SideBarNowPlaying.png" );
            case MyRecommendations:   return MooseUtils::icon( "recommended_radio" );
            case PersonalRadio:       return MooseUtils::icon( "personal_radio" );
            case LovedTracksRadio:    return MooseUtils::icon( "loved_radio" );
            case NeighbourhoodRadio:  return MooseUtils::icon( "neighbour_radio" );
            case RecentlyPlayed:      return MooseUtils::icon( "recent_tracks" );
            case RecentlyLoved:       return MooseUtils::icon( "recently_loved" );
            case RecentlyBanned:      return MooseUtils::icon( "recently_banned" );
            case MyTags:              return MooseUtils::icon( "my_tags" );
            case Friends:             return MooseUtils::icon( "my_friends" );
            case Neighbours:          return MooseUtils::icon( "my_neighbours" );
            #ifdef Q_WS_MAC
            case History:             return MooseUtils::icon( "history32" );
            #else
            case History:             return MooseUtils::icon( "history16" );
            #endif

            case RecentlyPlayedTrack: //FALL THROUGH
            case RecentlyLovedTrack:  //FALL THROUGH
            case RecentlyBannedTrack: return MooseUtils::icon( "icon_track" );
            case MyTagsChild:         return MooseUtils::icon( "icon_tag" );

            case FriendsChild:
            {
                if ( m_avatars.contains( index.data().toString() ) )
                    return m_avatars.value( index.data().toString() );

                return MooseUtils::icon( "user_blue" );
            }

            case NeighboursChild:
            {
                if ( m_avatars.contains( index.data().toString() ) )
                    return m_avatars.value( index.data().toString() );

                return MooseUtils::icon( "user_purple" );
            }

            case HistoryStation:      return MooseUtils::icon( "icon_radio" );

            default: break; //gcc warning--
        }

    QString const encoded_username = QUrl::toPercentEncoding( The::currentUsername() );

    if (role == StationUrlRole)
        switch (i.type()) 
        {
            #define encode( x ) QUrl::toPercentEncoding( x.value( index.row() ) )

            case MyProfile:
            case MyRecommendations:   return "lastfm://user/" + encoded_username + "/recommended";
            case PersonalRadio:       return "lastfm://user/" + encoded_username + "/personal";
            case LovedTracksRadio:    return "lastfm://user/" + encoded_username + "/loved";
            case NeighbourhoodRadio:  return "lastfm://user/" + encoded_username + "/neighbours";

            case RecentlyPlayedTrack:
            case RecentlyLovedTrack: 
            case RecentlyBannedTrack:
                //NOTE we can't know this without calling the TrackToId WebService request :(
                return QVariant();

            case MyTagsChild:
            {
                QString tagToEncode = m_tags.value( index.row() );
                return "lastfm://globaltags/" + QUrl::toPercentEncoding( 
                    tagToEncode.remove( tagToEncode.lastIndexOf( " (" ), tagToEncode.length() ) );
            }
            case FriendsChild:        return "lastfm://user/" + encode( m_friends ) + "/personal";
            case Neighbours:          return "lastfm://user/" + encode( m_neighbours ) + "/personal";
            case HistoryStation:      return m_history.value( index.row() ).url();

            default: break; //gcc warning--

            #undef encode
        }

    if (role == TrackRole) {
        Track track;
        switch (i.type()) {
            case RecentlyPlayedTrack: track = m_played.value( index.row() ); break;
            case RecentlyLovedTrack:  track = m_loved.value( index.row() ); break;
            case RecentlyBannedTrack: track = m_banned.value( index.row() ); break;

            default: break; //gcc warning--
        }

        if (!track.isNull()) {
            QVariantMap map;
            map["artist"] = QString(track.artist());
            map["title"] = track.title();
            return map;
        }
    }

  #ifdef Q_WS_MAC
    if (role == Qt::FontRole)
    {
        QFont font;
        font.setPixelSize( 11 );
        return font;
    }
  #endif

    if (role == UrlRole)
        if (i.type() == MyProfile)
            return "http://" + UnicornUtils::localizedHostName( The::settings().appLanguage() ) + "/user/" + encoded_username;
        else
            return data( index, StationUrlRole );

    return QVariant();
}


Qt::ItemFlags
SideBarModel::flags( const QModelIndex& index ) const
{
    SideBarItem i( index );
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;

    switch (i.type())
    {
        case StartAStation:
        case NowPlaying:
        case MyRecommendations:
        case PersonalRadio:
        case LovedTracksRadio:
        case NeighbourhoodRadio:
        case RecentlyPlayedTrack:
        case RecentlyLovedTrack:
        case RecentlyBannedTrack:
        case MyTagsChild:
        case FriendsChild:
        case NeighboursChild:
        case HistoryStation:
            flags |= Qt::ItemIsSelectable;
            break;

        default: break; //gcc warning--
    }

    if (i.type() == NowPlaying && !m_track_is_playing)
        flags &= ~(Qt::ItemIsEnabled | Qt::ItemIsSelectable); //disable

    switch (SideBarItem(i).classification())
    {
        case SideBarItem::User:
        case SideBarItem::Track:
        case SideBarItem::Tag:
            flags |= Qt::ItemIsDragEnabled;
            break;

        default: 
            break; //gcc warning--
    }

    return flags;
}


QModelIndex
SideBarModel::index( int row, int column, const QModelIndex& parent ) const
{
    return createIndex( row, column, parent.isValid() ? parent.row() : 0 );
}


QModelIndex
SideBarModel::parent( const QModelIndex& i ) const
{
    switch (i.internalId())
    {
        case 0:  return QModelIndex();
        default: return index( i.internalId(), 0 );
    }
}


int
SideBarModel::rowCount( const QModelIndex &parent ) const
{
    if (!parent.isValid())
        return SideBar::RowCount;

    // no second levels pls
    if (parent.internalId() > 0)
        return 0;

    switch (parent.row())
    {
        case RecentlyPlayed:
            return m_played.count();
        case RecentlyLoved:
            return m_loved.count();
        case RecentlyBanned:
            return m_banned.count();
        case MyTags:
            return m_tags.count();
        case Friends:
            return m_friends.count();
        case Neighbours:
            return m_neighbours.count();
        case History:
            return m_history.count();

        default:
            return 0;
    }
}


QStringList
SideBarModel::mimeTypes() const
{
     return QStringList() << "item/tag"
                          << "item/user"
                          << "item/artist"
                          << "item/track"
                          << "item/station"
                          ;
}


QMimeData*
SideBarModel::mimeData( const QModelIndexList& indexes ) const
{
    QModelIndex i = indexes.value( 0 );
    SideBarItem item( i );
    QMimeData *m = new QMimeData;

    switch (item.type())
    {
        case FriendsChild:
        case NeighboursChild:
        case MyProfile:
            m->setData( "item/user", i.data().toByteArray() );
            break;

        case MyRecommendations:
        case PersonalRadio:
        case LovedTracksRadio:
        case NeighbourhoodRadio:
        case HistoryStation:
            m->setData( "item/station", i.data( SideBar::StationUrlRole ).toByteArray() );
            break;

        case MyTags:
        case Friends:
        case Neighbours:
        case History:
        case RecentlyPlayed:
        case RecentlyLoved:
        case RecentlyBanned:
            return 0;

        case MyTagsChild:
            m->setData( "item/tag", i.data().toString().remove( QRegExp(" \\(\\d*\\)$") ).toUtf8() );
            break;

        case RecentlyBannedTrack:
        case RecentlyPlayedTrack:
        case RecentlyLovedTrack:
        {
            Track track = item.track();
            m->setData( "item/track", track.title().toUtf8() );
            m->setData( "item/artist", track.artist().name().toUtf8() );
            break;
        }

        default:
            break; //gcc warning--
    }

    return m;
}


template <class T> void
SideBarModel::changeData( int row, T& old_data, const T& new_data )
{
    QModelIndex const parent = index( row, 0 );
    int const n = old_data.count() - new_data.count();

    if (n > 0) beginRemoveRows( parent, 0, n - 1 );
    if (n < 0) beginInsertRows( parent, 0, -( n + 1 ) );
    old_data = new_data;
    if (n > 0) endRemoveRows();
    if (n < 0) endInsertRows();

    emit dataChanged( index( 0, 0, parent ), index( new_data.count() - 1, 0, parent ) );
}


void
SideBarModel::onResult( WsReply* r )
{
#if 0
    switch (r->type())
    {
        #define CASE( T ) case Type##T: { \
            T##Request *request = dynamic_cast<T##Request*>(r); \
            Q_ASSERT( request );

        #define break } break

        CASE( RecentTracks )
            changeData( SideBar::RecentlyPlayed, m_played, request->tracks() );
            break;

        CASE( RecentlyLovedTracks )
            changeData( SideBar::RecentlyLoved, m_loved, request->tracks() );
            break;

        CASE( RecentlyBannedTracks )
            changeData( SideBar::RecentlyBanned, m_banned, request->tracks() );
            break;

        CASE( UserTags )
//             m_tags.clear();
            WeightedStringList tags = (WeightedStringList)request->tags();
            for ( int i = 0; i < tags.count(); i++ )
                tags[i] += " (" + QVariant( tags.at( i ).weighting() ).toString() + ")";

            sortTags( tags, (SideBar::SortOrder) The::settings().currentUser().sideBarTagsSortOrder() );
            break;

        CASE( Friends )
            changeData( SideBar::Friends, m_friends, request->usernames() );

            if ( The::currentUser().isMetaDataEnabled() )
                queueAvatarsDownload( request->avatars() );

            break;

        CASE( Neighbours )
            sortNeighbours( request->usernames(), (SideBar::SortOrder) The::settings().currentUser().sideBarNeighbourSortOrder() );

            if ( The::currentUser().isMetaDataEnabled() )
                queueAvatarsDownload( request->avatars() );

            break;

    //////
        CASE( UserPictures )
            queueAvatarsDownload( request->urls() );
            break;

    //////
        CASE( Ban )
            beginInsertRows( index( SideBar::RecentlyBanned, 0 ), 0, 0 );
            m_banned.prepend( request->track() );
            endInsertRows();
            break;

        CASE( Love )
            beginInsertRows( index( SideBar::RecentlyLoved, 0 ), 0, 0 );
            m_loved.prepend( request->track() );
            endInsertRows();
            break;

        CASE( UnBan )
            int const n = m_banned.indexOf( request->track() );
            if (n != -1) {
                beginRemoveRows( index( SideBar::RecentlyBanned, 0 ), n, n );
                m_banned.removeAt( n );
                endRemoveRows();
            }
            else
                (new RecentlyBannedTracksRequest)->start();
            break;

        CASE( UnLove )
            int n = m_loved.indexOf( request->track() );
            if (n != -1) {
                beginRemoveRows( index( SideBar::RecentlyLoved, 0 ), n, n );
                m_loved.removeAt( n );
                endRemoveRows();
            }
            else
                (new RecentlyLovedTracksRequest)->start();
            break;

        #undef CASE
        #undef break

        default:
            break; //gcc warnings--
    }
#endif
}


void
SideBarModel::updateHistory()
{
  #ifndef HIDE_RADIO
    changeData( SideBar::History, m_history, The::currentUser().recentStations() );
  #endif
}


void
SideBarModel::sortTags( WeightedStringList tagsToSort, SideBar::SortOrder sortOrder )
{
    if ( sortOrder == SideBar::MostWeightOrder )
    {
        tagsToSort.weightedSort( Qt::DescendingOrder );
    }
    else if ( sortOrder == SideBar::AscendingOrder )
    {
        tagsToSort.sort();
    }
    else if ( sortOrder == SideBar::DescendingOrder )
    {
        tagsToSort.sort( Qt::DescendingOrder );
    }

    changeData( SideBar::MyTags, m_tags, tagsToSort );
}


void
SideBarModel::sortTags( SideBar::SortOrder sortOrder )
{
    sortTags( m_tags, sortOrder );
}


bool caseInsensitiveLessThan(const QString &s1, const QString &s2)
{
    return s1.toLower() < s2.toLower();
}


void
SideBarModel::sortNeighbours( QStringList neighboursToSort, SideBar::SortOrder sortOrder )
{
    if ( m_neighbours.count() == 0 )
    {
        m_neighboursBySimilarity = neighboursToSort;
    }

    if ( sortOrder == SideBar::MostWeightOrder ) 
    {
        neighboursToSort = m_neighboursBySimilarity;
    }
    else if ( sortOrder == SideBar::AscendingOrder )
    {
        qSort( neighboursToSort.begin(), neighboursToSort.end(), caseInsensitiveLessThan );
    }
    else if ( sortOrder == SideBar::DescendingOrder )
    {
        qSort( neighboursToSort.begin(), neighboursToSort.end(), qGreater<QString>() );
    }

    changeData( SideBar::Neighbours, m_neighbours, neighboursToSort );
}


void
SideBarModel::sortNeighbours( SideBar::SortOrder sortOrder )
{
    sortNeighbours( m_neighbours, sortOrder );
}


void
SideBarModel::queueAvatarsDownload( const QMap<QString, QString>& urls )
{
    bool start = m_avatarQueue.isEmpty();
    m_avatarQueue.unite( urls );

    QMutableMapIterator<QString, QString> i( m_avatarQueue );
    while (i.hasNext())
    {
        i.next();

        QString const name = i.key();
        QString const url = i.value();

        if ( !QUrl( url ).host().startsWith( USER_AVATAR_HOST ) )
        {
            // Don't download avatar if it's just the default blank avatar!
            // but do if it's the current username since we have to show something at the top there
            if ( name != The::currentUsername() )
                i.remove();
        }
    }

    if ( start )
        downloadAvatar( m_avatarQueue.keys().value( 0 ), m_avatarQueue.values().value( 0 ) );
}


#include <QtNetwork/QHttp>
struct AvatarDownloader : QHttp
{
    QString username;
};


void
SideBarModel::downloadAvatar( const QString& user, const QUrl& url )
{
    AvatarDownloader* http = new AvatarDownloader;
    http->setHost( url.host() );
    http->get( url.encodedQuery().isEmpty()
            ? url.path()
            : url.path() + "?" + url.encodedQuery() ); //TODO cache

    http->username = user;

    connect( http, SIGNAL( dataAvailable( QByteArray ) ), SLOT( onAvatarDownloaded( QByteArray ) ) );
}


void
SideBarModel::onAvatarDownloaded( QByteArray const buffer )
{
    QString const username = static_cast<AvatarDownloader*>(sender())->username;

    QPixmap avatar;
    bool ok = avatar.loadFromData( buffer );

    if ( ok && !avatar.isNull() && avatar.height() > 0 && avatar.width() > 0 )
    {
        if ( username.toLower() == The::currentUsername().toLower() )
        {
            #ifdef LINUX
            int m = 38;
            #else
            int m = 30;
            #endif

            m_avatar = avatar.scaled( m, m, Qt::KeepAspectRatio, Qt::SmoothTransformation );
            emitRowChanged( SideBar::MyProfile );
        }
        else
        {
            int m = 22;
            avatar = avatar.scaled( m, m, Qt::KeepAspectRatio, Qt::SmoothTransformation );

            // This code is here to stop Qt from crashing on certain weirdly shaped avatars.
            // We had a case were an avatar got a height of 1px after scaling and it would
            // crash in the rendering code. This here just fills in the background with
            // transparency first.
            if ( avatar.width() < m || avatar.height() < m )
            {
                QImage finalAvatar( m, m, QImage::Format_ARGB32 );
                finalAvatar.fill( 0 );

                QPainter p( &finalAvatar );
                QRect r;

                if ( avatar.width() < m )
                    r = QRect( ( m - avatar.width() ) / 2, 0, avatar.width(), avatar.height() );
                else
                    r = QRect( 0, ( m - avatar.height() ) / 2, avatar.width(), avatar.height() );

                p.drawPixmap( r, avatar );
                p.end();

                avatar = QPixmap::fromImage( finalAvatar );
            }

            if ( !avatar.isNull() && avatar.height() > 0 && avatar.width() > 0 )
            {
                m_avatars.insert( username, avatar );
                emitRowChanged( SideBar::Friends );
            }
        }
    }
    else
        qCritical() << "Loading of avatar image from QByteArray failed for user: " << username;

    sender()->deleteLater();

    m_avatarQueue.remove( username );
    if ( m_avatarQueue.count() )
        downloadAvatar( m_avatarQueue.keys().value( 0 ), m_avatarQueue.values().value( 0 ) );
}


void
SideBarModel::onAppEvent( int event, const QVariant& data )
{
    //TODO
#if 0
    switch (event)
    {
        case Event::UserChanged:
        {
            QString const name = data.toString();

            m_friends.clear();
            m_neighbours.clear();
            m_banned.clear();
            m_loved.clear();
            m_played.clear();
            m_tags.clear();
            m_avatar = QPixmap();

            reset();

        #ifndef HIDE_RADIO
            disconnect( &The::currentUser(), 0, this, 0 );
            connect( &The::currentUser(), SIGNAL(historyChanged()), SLOT(updateHistory()) );
            updateHistory();
        #endif

            break;
        }

        case Event::TuningIn:
        case Event::PlaybackStarted:
        {
            TrackInfo track = data.value<TrackInfo>();

            if (track.source() != TrackInfo::Radio)
                m_change_station_text = tr("Start a Station");
            else
                m_change_station_text = tr("Change Station");

            m_track_is_playing = true;

            QModelIndex const i = index( StartAStation, 0 );
            emit dataChanged( i, i.sibling( i.row() + 1, 0 ) );
            return;
        }
        break;

        case Event::PlaybackEnded:
        {
            m_track_is_playing = false;
            QModelIndex const i = index( NowPlaying, 0 );
            emit dataChanged( i, i );
        }
        break;
    }
#endif
}


void
SideBarModel::emitRowChanged( int parent_row, int child_row /*= -1*/ )
{
    QModelIndex parent;
    if (child_row != -1)
        parent = index( parent_row, 0 );

    QModelIndex i = index( child_row, 0, parent );

    emit dataChanged( i, i );
}



/////////////////
// SideBarItem //
/////////////////

SideBarItem::SideBarItem( const QModelIndex& i ) :
        m_classification( ClassNone ),
        m_type( TypeUnknown ),
        m_index( i )
{
    if (i.parent().isValid())
        switch (i.parent().row())
        {
            case RecentlyPlayed: m_type = RecentlyPlayedTrack; break;
            case RecentlyLoved:  m_type = RecentlyLovedTrack; break;
            case RecentlyBanned: m_type = RecentlyBannedTrack; break;
            case MyTags:         m_type = MyTagsChild; break;
            case Friends:        m_type = FriendsChild; break;
            case Neighbours:     m_type = NeighboursChild; break;
            case History:        m_type = HistoryStation; break;

            default: break; //gcc warning--
        }
    else
        m_type = (SideBar::Type)i.row();

//////
    switch (m_type)
    {
        case RecentlyPlayedTrack:
        case RecentlyLovedTrack:
        case RecentlyBannedTrack:
            m_classification = Track;
            break;

        case MyTagsChild:
            m_classification = Tag;
            break;

        case MyProfile:
        case FriendsChild:
        case NeighboursChild:
            m_classification = User;
            break;

        case MyRecommendations:
        case PersonalRadio:
        case LovedTracksRadio:
        case NeighbourhoodRadio:
        case HistoryStation:
            m_classification = Station;
            break;

        default: break; //gcc warning--
    }
};


QIcon
SideBarItem::icon() const
{
    return m_index.data( Qt::DecorationRole ).value<QIcon>();
}


Track
SideBarItem::track() const
{
    QVariantMap map = m_index.data( SideBar::TrackRole ).toMap();

    MutableTrack track;
    track.setArtist( map["artist"].toString() );
    track.setTitle( map["title"].toString() );
    return track;
}


Station
SideBarItem::station() const
{
    ::Station station( m_index.data( StationUrlRole ).toString() );
    station.setTitle( m_index.data().toString() );
    return station;
}

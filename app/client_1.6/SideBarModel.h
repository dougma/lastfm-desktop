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


#ifndef SIDE_BAR_MODEL_H
#define SIDE_BAR_MODEL_H

#include <QAbstractItemModel>
#include <QStringList>
#include <QPixmap>
#include <QIcon>
#include <QHash>

#include "RadioEnums.h"
#include "lib/lastfm/core/WeightedStringList.h"


namespace SideBar
{
    enum Type
    {
        MyProfile = 0,
        Spacer1, /**/
        StartAStation,
        NowPlaying,
        Spacer2, /**/
        MyRecommendations,
        PersonalRadio,
        LovedTracksRadio,
        NeighbourhoodRadio,
        Spacer3, /**/
        RecentlyPlayed,
        RecentlyLoved,
        RecentlyBanned,
        MyTags,
        Friends,
        Neighbours,
        History,
        
        /// don't touch me!
        RowCount,
        
        FriendsChild,
        NeighboursChild,
        MyTagsChild,
        RecentlyBannedTrack,
        RecentlyPlayedTrack,
        RecentlyLovedTrack,
        HistoryStation,
        
        /// don't touch me!
        TypeUnknown
    };
    
    enum Role
    {
        StationUrlRole = Qt::UserRole,
        UrlRole,
        TrackRole
    };
    
    enum SortOrder
    {
        MostWeightOrder,
        AscendingOrder,
        DescendingOrder
    };
}


class QUrl;

class SideBarModel : public QAbstractItemModel
{
    Q_OBJECT
    
public:
    SideBarModel();
    
    void addRecentlyPlayedTrack( Track );
    
    /** Reimplimentations */
    QVariant data(const QModelIndex &index, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    Qt::ItemFlags flags( const QModelIndex& ) const;
    QMimeData *mimeData( const QModelIndexList& indexes ) const;
    QStringList mimeTypes() const;
    
    /** no header, one column always */
    QVariant headerData( int, Qt::Orientation, int ) const { return QVariant(); }
    int columnCount( const QModelIndex& ) const { return 1; }
    
    void sortTags( WeightedStringList tagsToSort, SideBar::SortOrder sortOrder );
    void sortTags( SideBar::SortOrder sortOrder );
    void sortNeighbours( QStringList neighboursToSort, SideBar::SortOrder );
    void sortNeighbours( SideBar::SortOrder sortOrder );
    
private slots:
    void onResult( Request* );
    void onAvatarDownloaded( QByteArray );
    void onAppEvent( int, const QVariant& );
    void updateHistory();
    
private:
    QStringList m_friends;
    QStringList m_neighbours;
    QStringList m_neighboursBySimilarity;
    
    QList<Track> m_banned;
    QList<Track> m_loved;
    QList<Track> m_played;
    WeightedStringList m_tags;
    QList<Station> m_history;

    void queueAvatarsDownload( const QMap<QString, QString>& urls );
    void downloadAvatar( const QString& user, const QUrl& url );
    void emitRowChanged( int parent_row, int child_row = -1 );
    
    template <class T> void changeData( int row, T& old_data, const T& new_data );
    
    QPixmap m_avatar;
    
    bool m_track_is_playing;
    
    QString m_change_station_text;
    QMap<QString, QString> m_avatarQueue;
    QHash<QString, QIcon> m_avatars;
};


class SideBarItem
{
public:
    SideBarItem( const QModelIndex& );
    
    enum Classification
    {
        Station,
        Track,
        Tag,
        User,
        ClassNone
    };
    
    SideBar::Type type() const { return m_type; }
    Classification classification() const { return m_classification; }
    QIcon icon() const;
    ::Track track() const;
    ::Station station() const;
    
private:
    Classification m_classification;
    SideBar::Type m_type;
    
    QModelIndex m_index;
};

#endif

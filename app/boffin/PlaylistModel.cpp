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

#include "PlaylistModel.h"

PlaylistModel::PlaylistModel( QObject* p )
              :QAbstractItemModel( p ),
               m_tracks()
{}


int 
PlaylistModel::columnCount( const QModelIndex& parent ) const
{ 
    if( parent.isValid())
        return 0;
    else
        return 3; 
}

int 
PlaylistModel::rowCount( const QModelIndex& parent ) const
{ 
    if( parent.isValid() )
        return 0;
    else
        return m_tracks.size(); 
}

QVariant 
PlaylistModel::headerData( int section, Qt::Orientation, int role ) const
{ 
    if( role != Qt::DisplayRole ) return QVariant();
    
    switch( section ) 
    {
        case 0: return tr( "Artist" );
        case 1: return tr( "Title" ); 
        case 2: return tr( "Url" );
    }

    return QVariant(); 
}


QVariant 
PlaylistModel::data( const QModelIndex& index, int role ) const
{
    if( role != Qt::DisplayRole || 
        index.row() >= m_tracks.size() ||
        index.column() >= columnCount( index.parent())) 
        return QVariant();

    Track t = m_tracks[ index.row() ]; 
    switch( index.column() )
    {
        case 0: return QString(t.artist());
        case 1: return t.title();
        case 2: return t.url().toString();
        default: return QVariant();
    }
}

QModelIndex 
PlaylistModel::index( int row, int column, const QModelIndex& p) const
{
    if ( row < m_tracks.size() && 
         column < columnCount(p) && 
         !p.isValid() )
    {
        return createIndex( row, column );
    }
    else 
    {
        return  QModelIndex();
    }
        
}

void 
PlaylistModel::addTracks( QList< Track > tracks )
{
    if( !tracks.isEmpty() )
    {
        beginInsertRows( QModelIndex(), m_tracks.size(), m_tracks.size() + tracks.size() -1 );
        m_tracks <<  tracks; 
        endInsertRows();
    }
}

void 
PlaylistModel::clear()
{
    beginRemoveRows( QModelIndex(), 0, m_tracks.size() -1 ); 
    m_tracks.clear(); 
    endRemoveRows(); 
}


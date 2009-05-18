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

#ifndef PLAYLIST_MODEL_H_
#define PLAYLIST_MODEL_H_

#include <QAbstractItemModel>
#include <lastfm/Track>

class PlaylistModel : public QAbstractItemModel
{
Q_OBJECT
public:
    PlaylistModel( QObject* p = 0 );
    virtual int columnCount( const QModelIndex& parent = QModelIndex()) const;
    virtual int rowCount( const QModelIndex& parent = QModelIndex() ) const;
    
    virtual QVariant headerData( int section, Qt::Orientation, int role = Qt::DisplayRole ) const;
    virtual QVariant data( const QModelIndex& index, int role ) const;

    virtual QModelIndex index( int row, int column, const QModelIndex& p = QModelIndex()) const;

    virtual QModelIndex parent( const QModelIndex& index ) const
    { return QModelIndex(); }


    void addTracks( QList< Track > tracks );
    void clear();


private:
    QList< Track > m_tracks;
};

#endif //PLAYLIST_MODEL_H_


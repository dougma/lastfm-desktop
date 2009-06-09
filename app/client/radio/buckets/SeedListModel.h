/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef SEED_LIST_MODEL_H
#define SEED_LIST_MODEL_H

#include <QAbstractListModel>

class Seed;

class SeedListModel : public QAbstractListModel
{
Q_OBJECT
public:
    SeedListModel( QObject* parent = 0 ):QAbstractListModel( parent ){}
    
    void addItem( Seed* );
    void clear();
    
    
    virtual QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
    virtual int rowCount( const QModelIndex& parent = QModelIndex() ) const;
    virtual bool setData( const QModelIndex&, const QVariant&, int role = Qt::EditRole );
    virtual Qt::ItemFlags flags( const QModelIndex& index ) const;
    virtual QMimeData* mimeData( const QModelIndexList& indexes ) const;
    
    Seed* itemFromIndex( const QModelIndex& i ) const
    {
        return m_items[ i.row() ];
    }
    
    QList< Seed* > findSeeds( const QString& string ) const;
    
private:
    QList< Seed* > m_items;
    
private slots:
    void onSeedDestroyed();
    void onSeedUpdated();
};

#endif //SEED_LIST_MODEL_H
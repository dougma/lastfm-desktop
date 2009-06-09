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
#include "SeedListModel.h"
#include "Seed.h"
#include "PlayableMimeData.h"

QList< Seed* > 
SeedListModel::findSeeds( const QString& string ) const
{
    QList< Seed* > seeds;
    foreach( Seed* s, m_items )
    {
        if( s->name().compare( string, Qt::CaseInsensitive ))
            continue;
        
        seeds << s;
    }
    
    return seeds;
}


void 
SeedListModel::addItem( Seed* s )
{
    beginInsertRows( QModelIndex(), rowCount(), rowCount());
    m_items << s;
    connect( s, SIGNAL( updated()), SLOT( onSeedUpdated()));
    connect( s, SIGNAL( destroyed()), SLOT( onSeedDestroyed()));
    endInsertRows();
}


void 
SeedListModel::clear()
{
    beginRemoveRows( QModelIndex(), 0, rowCount());
    m_items.clear();
    endRemoveRows();
}


QVariant 
SeedListModel::data( const QModelIndex& index, int role ) const
{
    switch ( role ) 
    {
        case Qt::DisplayRole: return m_items[ index.row() ]->name();
        case Qt::DecorationRole: return m_items[ index.row() ]->icon();
        case moose::TypeRole: return m_items[ index.row() ]->playableType();
        default: return QVariant();
    }
}


int 
SeedListModel::rowCount( const QModelIndex& parent ) const
{ 
    //This is a one-dimensional list - there should be no parents
    if( parent.isValid() )
        return 0; 
    
    return m_items.size(); 
}


bool 
SeedListModel::setData( const QModelIndex& index, const QVariant& data, int role )
{
    switch( role )
    {
        case Qt::DecorationRole:
            m_items[ index.row() ]->setIcon( data.value<QIcon>());
            emit dataChanged( index, index );
            return true;
        default:
            return false;
    }
}


void 
SeedListModel::onSeedDestroyed()
{
    Seed* s = static_cast<Seed*>(sender() );
    Q_ASSERT( s );
    
    int i;
    while(( i = m_items.indexOf( s )) >= 0)
    {
        beginRemoveRows( QModelIndex(), i, i );
        m_items.removeAt( i );
        endRemoveRows();
    }
}


void 
SeedListModel::onSeedUpdated()
{
    Seed* s = static_cast<Seed*>(sender() );
    Q_ASSERT( s );
    QModelIndex i = createIndex( m_items.indexOf( s ), 0);
    emit dataChanged( i, i );


}


Qt::ItemFlags 
SeedListModel::flags( const QModelIndex& i ) const
{
    return (QAbstractListModel::flags( i ) | Qt::ItemIsDragEnabled);
}


QMimeData* 
SeedListModel::mimeData( const QModelIndexList& indexes ) const
{
    PlayableMimeData* data = new PlayableMimeData;
    Seed* item = m_items[ indexes.first().row() ];
    data->setType( item->playableType() );
    data->setText( item->name() );
    data->setImageData( QVariant::fromValue<QImage>(item->icon().pixmap( QSize( 126, 100 )).toImage()) );
    
    if( item->playableType() == Seed::PreDefinedType )
        data->setRQL( item->rql() );
    
    return data;
}

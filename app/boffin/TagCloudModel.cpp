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
 
#include "TagCloudModel.h"
#include "lib/lastfm/core/CoreDir.h"
#include <QSqlQuery>
#include <QDebug>

TagCloudModel::TagCloudModel( QObject* parent )
              :QAbstractItemModel( parent )
{
    m_db = QSqlDatabase::addDatabase( "QSQLITE", "TagCloud");
    m_db.setDatabaseName( CoreDir::data().path() + "/LocalCollection.db" );
    m_db.open();
}


TagCloudModel::~TagCloudModel()
{
    m_tagHash.clear();
}


int 
TagCloudModel::rowCount( const QModelIndex& parent ) const
{
    if( parent.isValid() )
        return 0;
    
    return m_tagHash.count();
}


QVariant 
TagCloudModel::data( const QModelIndex& index, int role ) const
{
    switch( role )
    {
        case Qt::DisplayRole:
        {
            QHash< QString, float>::const_iterator i = m_tagHash.constBegin();
            i += index.row();
            return i.key();
        }
        
        case TagCloudModel::WeightRole:
        {
            QHash< QString, float>::const_iterator i = m_tagHash.constBegin();
            i += index.row();
            return ((i.value() - m_minWeight) / m_totalWeight);
        }

        default:
            return QVariant();
    }
}


void 
TagCloudModel::fetchTags()
{
    QSqlQuery query( "select name, tag, sum(weight) from tracktags join tags "
                     "where tracktags.tag = tags.id group by tags.id order by sum(weight) desc "
                     "limit 40"
                        , m_db );
    m_tagHash.clear();
    m_totalWeight = 0;
    m_minWeight = 65535;
    while( query.next())
    {
        const float weight = query.value( 2 ).value<float>();
        m_minWeight = weight < m_minWeight ? weight : m_minWeight;
        m_totalWeight += weight;
        m_tagHash.insert( query.value( 0 ).toString(), weight );
    }
    reset();
}

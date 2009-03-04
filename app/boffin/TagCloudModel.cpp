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
#include "app/clientplugins/localresolver/LocalCollection.h"
#include "app/clientplugins/localresolver/QueryError.h"
#include <float.h> 
#include <math.h>
#include <QDebug>


TagCloudModel::TagCloudModel( QObject* parent, int limit )
              :QAbstractItemModel( parent ),
               m_limit( limit )
{
    m_collection = LocalCollection::create( QString( "TagCloudModel"));
    fetchTags();
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
            QMultiMap< float, QString >::const_iterator i = m_tagHash.constEnd();
            i -= index.row() + 1;
            return i.value();
        }
                
        case TagCloudModel::WeightRole:
        {
            QMultiMap< float, QString>::const_iterator i = m_tagHash.constEnd();
            i -= index.row() + 1;
            return QVariant::fromValue<float>((i.key() / m_maxWeight));
        }

        case TagCloudModel::LinearWeightRole:
        {
            QMultiMap< float, QString >::const_iterator i = m_logTagHash.constEnd();
            i -= index.row() + 1;
            return QVariant::fromValue<float>( ( i.key() - m_minLogWeight ) / ((m_logTagHash.constEnd() -1 ).key() - m_minLogWeight));
        }

        default:
            return QVariant();
    }
}


Qt::ItemFlags 
TagCloudModel::flags( const QModelIndex & index ) const
{
    Q_UNUSED( index );
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}


void 
TagCloudModel::fetchTags()
{
    m_tagHash.clear();
    m_logTagHash.clear();

    m_maxWeight = 0;

    typedef QPair< QString, float > Pair;

    try 
    {
        QList< Pair > tags = m_collection->getTopTags( m_limit );
        m_minLogWeight = FLT_MAX;
        foreach(const Pair& tag, tags)
        {
            m_maxWeight = qMax( tag.second, m_maxWeight );
            float logWeight = log( tag.second );
            m_minLogWeight = logWeight < m_minLogWeight ? logWeight : m_minLogWeight;
            m_logTagHash.insert( logWeight, tag.first );
            m_tagHash.insert( tag.second, tag.first );
        }
    }
    catch (QueryError e) 
    {
        qDebug() << e.text();
    }

    reset();
}

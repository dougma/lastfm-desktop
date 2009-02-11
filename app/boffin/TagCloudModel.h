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
 
#ifndef TAG_CLOUD_MODEL_H
#define TAG_CLOUD_MODEL_H

#include <QAbstractItemModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QHash>

class TagCloudModel: public QAbstractItemModel
{
public:
    enum CustomRoles { WeightRole = Qt::UserRole };

    TagCloudModel( QObject* parent = 0 );
    ~TagCloudModel();

    virtual QModelIndex index( int row, int column, const QModelIndex& parent ) const
    {
        if( parent.isValid() ) return QModelIndex();
        return createIndex( row, column );
    }
    
    virtual QModelIndex parent( const QModelIndex& ) const{ return QModelIndex(); }
    virtual int rowCount( const QModelIndex& = QModelIndex() ) const;
    virtual int columnCount( const QModelIndex& =QModelIndex() ) const { return 1; }
    virtual QVariant data( const QModelIndex&, int role = Qt::DisplayRole ) const;
    
    void fetchTags();

protected:
    QSqlDatabase m_db;
    QHash< QString, float> m_tagHash;
    float m_totalWeight;
    float m_minWeight;
};

#endif //TAG_CLOUD_MODEL_H


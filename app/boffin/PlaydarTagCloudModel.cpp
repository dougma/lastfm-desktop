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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <float.h>
#include <math.h>
#include "PlaydarTagCloudModel.h"
#include "PlaydarConnection.h"
#include <QTimer>

PlaydarTagCloudModel::PlaydarTagCloudModel(PlaydarConnection* playdar)
:m_playdar(playdar)
,m_loadingTimer( 0 )
{
}

PlaydarTagCloudModel::~PlaydarTagCloudModel()
{
}

void
PlaydarTagCloudModel::startGetTags(const QString& rql)
{
    m_hosts.clear();
    m_tagListBuffer.clear();
    m_tagList.clear();

    m_maxWeight = 0;
    m_maxLogWeight = FLT_MIN;
    m_minLogWeight = FLT_MAX;

    BoffinTagRequest* req = m_playdar->boffinTagcloud(rql);
    connect(req, SIGNAL(tagItem(BoffinTagItem)), SLOT(onTag(BoffinTagItem)));
    connect(req, SIGNAL(tagItem(BoffinTagItem)), SIGNAL(tagItem(BoffinTagItem)));
    connect(req, SIGNAL(error()), this, SLOT(onTagError()));
    if( m_loadingTimer )
    {
    	delete( m_loadingTimer );
    	m_loadingTimer = 0;
    }
    qDebug() << "Fetching rql: " << rql;
}

void
PlaydarTagCloudModel::onTag(BoffinTagItem tag)
{
	if( m_loadingTimer )
		m_loadingTimer->stop();
	do {
		// check if the host is being filtered.
		if (m_hostFilter.contains(tag.m_host))
			break;

        m_hosts.insert(tag.m_host);

		//Merge any existing tags
		if( int i = m_tagListBuffer.indexOf( tag ) >= 0 )
		{
			m_tagListBuffer[ i ].m_weight += tag.m_weight;
			m_tagListBuffer[ i ].m_logWeight = log( m_tagListBuffer[i].m_weight );
			m_maxWeight = qMax( m_tagListBuffer[i].m_weight, m_maxWeight);
			m_maxLogWeight = qMax( m_tagListBuffer[i].m_logWeight, m_maxLogWeight);
			emit dataChanged( createIndex( i, 0), createIndex( i, 0));
			break;
		}

	//	beginInsertRows( QModelIndex(), m_tagListBuffer.size(), m_tagListBuffer.size() + 1);
			tag.m_logWeight = log( tag.m_weight );
			m_tagListBuffer << tag;
			m_maxWeight = qMax( tag.m_weight, m_maxWeight );
			m_maxLogWeight = qMax( m_maxLogWeight, tag.m_logWeight );
			m_minLogWeight = qMin( m_minLogWeight, tag.m_logWeight );
	//	endInsertRows();
	} while( false );

	if( !m_loadingTimer )
	{
		m_loadingTimer = new QTimer();
		m_loadingTimer->setSingleShot( true );
		connect( m_loadingTimer, SIGNAL( timeout()), SLOT(onFetchedTags()));
	}
    m_loadingTimer->start( 1000 );

}

void
PlaydarTagCloudModel::onFetchedTags()
{
	m_loadingTimer->deleteLater();
	m_loadingTimer = 0;
	beginInsertRows( QModelIndex(), m_tagList.size(), m_tagListBuffer.size() );
		m_tagList = m_tagListBuffer;
		qSort( m_tagList.begin(), m_tagList.end(), qGreater<BoffinTagItem >() );
	endInsertRows();
	emit fetchedTags();
}

void
PlaydarTagCloudModel::onTagError()
{
}

// virtual
QVariant
PlaydarTagCloudModel::data( const QModelIndex& index, int role ) const
{
	if( index.row() >= m_tagList.count() )
		return QVariant();

	switch( role )
    {
        case Qt::DisplayRole:
        {
            QList< BoffinTagItem >::const_iterator i = m_tagList.constBegin();
            i += index.row();
            return i->m_name;
        }

        case PlaydarTagCloudModel::WeightRole:
        {
            QList< BoffinTagItem >::const_iterator i = m_tagList.constBegin();
            i += index.row();
            return QVariant::fromValue<float>((i->m_weight / m_maxWeight));
        }

        case PlaydarTagCloudModel::LinearWeightRole:
        {
            if (m_tagList.count() == 1) {
                return 1;
            }
            QList< BoffinTagItem >::const_iterator i = m_tagList.constBegin();
            i += index.row();
            return QVariant::fromValue<float>( ( i->m_logWeight - m_minLogWeight ) / (m_maxLogWeight - m_minLogWeight));
        }

        case PlaydarTagCloudModel::CountRole:
        {
            QList< BoffinTagItem >::const_iterator i = m_tagList.constBegin();
            i += index.row();
            return QVariant::fromValue<int>( i->m_count );
        }

        default:
            return QVariant();
    }
}


// virtual
QModelIndex
PlaydarTagCloudModel::index( int row, int column, const QModelIndex& parent /*= QModelIndex()*/) const
{
    return parent.isValid() || row > m_tagList.count() ?
        QModelIndex() :
        createIndex( row, column );
}

QModelIndex
PlaydarTagCloudModel::indexOf( const BoffinTagItem& tag )
{
	return createIndex( m_tagList.indexOf( tag ), 0 );
}

//virtual
QModelIndex
PlaydarTagCloudModel::parent( const QModelIndex& ) const
{
    return QModelIndex();
}

//virtual
int
PlaydarTagCloudModel::rowCount( const QModelIndex& p ) const
{
    if( p.isValid())
        return 0;

    return m_tagList.count();
}

//virtual
int
PlaydarTagCloudModel::columnCount( const QModelIndex& p ) const
{
    if( p.isValid() )
        return 0;
    return 1;
}

void
PlaydarTagCloudModel::setHostFilter(QSet<QString> hosts)
{
    m_hostFilter = hosts;
//    onTags(m_tags);         // recalc things...
}

void
PlaydarTagCloudModel::addToHostFilter(const QString& hostname)
{
    m_hostFilter.insert(hostname);
//    onTags(m_tags);
}

void
PlaydarTagCloudModel::setTagMapping(QMap<QString, QString> tagMap)
{
//    m_tagMap = tagMap;
//    onTags(m_tags);
}



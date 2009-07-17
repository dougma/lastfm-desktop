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
#include <float.h>
#include <math.h>
#include "PlaydarTagCloudModel.h"
#include "playdar/PlaydarConnection.h"
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
    m_maxLogCount = FLT_MIN;
    m_minLogCount = FLT_MAX;
    m_maxTrackCount = 0;
    m_totalTracks = 0;
    m_totalDuration = 0;

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

    // check if the host is being filtered.
    if (!m_hostFilter.contains(tag.m_host)) {
        m_hosts.insert(tag.m_host);

		if( int i = m_tagListBuffer.indexOf( tag ) >= 0 )
		{
    		// merge into existing tag
			m_tagListBuffer[ i ].m_weight += tag.m_weight;
            m_tagListBuffer[ i ].m_count += tag.m_count;
			m_tagListBuffer[ i ].m_logCount = log( (float) m_tagListBuffer[i].m_count );
			m_maxWeight = qMax( m_tagListBuffer[i].m_weight, m_maxWeight);
			m_maxLogCount = qMax( m_tagListBuffer[i].m_logCount, m_maxLogCount);
        }
        else
        {
            // new tag
		    tag.m_logCount = log( (float) tag.m_count );
		    m_tagListBuffer << tag;
		    m_maxTrackCount = qMax( tag.m_count, m_maxTrackCount );
		    m_maxWeight = qMax( tag.m_weight, m_maxWeight );
		    m_maxLogCount = qMax( m_maxLogCount, tag.m_logCount );
		    m_minLogCount = qMin( m_minLogCount, tag.m_logCount );
        }
	}

    // fire onFetchedTags after 1 second of idle-ness
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
//	beginInsertRows( QModelIndex(), m_tagList.size(), m_tagListBuffer.size() );
		m_tagList = m_tagListBuffer;
		qSort( m_tagList.begin(), m_tagList.end(), qGreater<BoffinTagItem >() );
//	endInsertRows();
    reset();
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

    QList< BoffinTagItem >::const_iterator i = m_tagList.constBegin();
    i += index.row();

	switch( role )
    {
        case Qt::DisplayRole:
            return i->m_name;

        case PlaydarTagCloudModel::WeightRole:
            return QVariant::fromValue<float>((i->m_weight / m_maxWeight));

        case PlaydarTagCloudModel::LinearWeightRole:
            if (m_maxLogCount == m_minLogCount) {
                return 1;
            }
            return QVariant::fromValue<float>( ( i->m_logCount - m_minLogCount ) / (m_maxLogCount - m_minLogCount));

        case PlaydarTagCloudModel::CountRole:
            return QVariant::fromValue<int>(i->m_count);

        case PlaydarTagCloudModel::SecondsRole:
            return QVariant::fromValue<int>(i->m_seconds);

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


int
PlaydarTagCloudModel::maxTrackCount() const
{
	return m_maxTrackCount;
}


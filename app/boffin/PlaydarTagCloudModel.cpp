	#include <float.h>
#include <math.h>
#include "PlaydarTagCloudModel.h"
#include "PlaydarConnection.h"


PlaydarTagCloudModel::PlaydarTagCloudModel(PlaydarConnection *playdar)
:m_playdar(playdar)
,m_minLogWeight( FLT_MAX )
,m_maxWeight( 0 )
,m_maxLogWeight( 0 )
{
}

PlaydarTagCloudModel::~PlaydarTagCloudModel()
{
}

void
PlaydarTagCloudModel::startGetTags(const QString& rql)
{
	//TODO: reset max/minweight member variables and clear current
	//		tag results.

    BoffinTagRequest* req = m_playdar->boffinTagcloud(rql);
    connect(req, SIGNAL(tagItem(BoffinTagItem)), this, SLOT(onTag(BoffinTagItem)));
    connect(req, SIGNAL(error()), this, SLOT(onTagError()));
}

void
PlaydarTagCloudModel::onTag(BoffinTagItem tag)
{
    if( int i = m_tagList.indexOf( tag ) >= 0 )
    {
    	m_tagList[ i ].m_weight += tag.m_weight;
    	m_tagList[ i ].m_logWeight = log( m_tagList[i].m_weight );
    	m_maxWeight = qMax( m_tagList[i].m_weight, m_maxWeight);
    	m_maxLogWeight = qMax( m_tagList[i].m_logWeight, m_maxLogWeight);
    	emit dataChanged( createIndex( i, 0), createIndex( i, 0));
    	return;
    }

	// check if the host is being filtered.
    if (m_hostFilter.contains(tag.m_host))
		return;

	beginInsertRows( QModelIndex(), m_tagList.size(), m_tagList.size() + 1);
		tag.m_logWeight = log( tag.m_weight );
		m_tagList << tag;
		m_maxWeight = qMax( tag.m_weight, m_maxWeight );
        m_maxLogWeight = qMax( m_maxLogWeight, tag.m_logWeight );
		m_minLogWeight = qMin( m_minLogWeight, tag.m_logWeight );
	endInsertRows();

//	TagMapIterator it = m_tagHash.
//	if (it == tagWeightMap.end()) {
//		// first instance of this tag
//		tagWeightMap.insert(tag.m_name, tag.m_weight);
//	} else {
//		// multiple hosts have this tag; add their weights:
//		it.value() += tag.m_weight;
//	}
//
//    beginInsertRows( QModelIndex(), m_tagHash.size(), m_tagHash.size() + tagWeightMap.size());
//    int lastChangedIndex;
//    int firstChangedIndex = lastChangedIndex = m_tagHash.size() - 1;
//
//    for (TagWeightMapIterator it = tagWeightMap.begin(); it != tagWeightMap.end(); it++)
//    {
//        float& weight = it.value();
//        const QString& name = it.key();
//
//        m_maxWeight = qMax( weight, m_maxWeight );
//        float logWeight = log( weight );
//        if( logWeight < m_minLogWeight)
//        {
//        	m_minLogWeight = logWeight;
//        	lastChangedIndex = m_tagHash.size();
//        	firstChangedIndex = 0;
//        	//the minLogWeight will effect other data in the model
//        }
//        m_logTagHash.insert( logWeight, name );
//        m_tagHash.insert( weight, name );
//    }
//    endInsertRows();
//    if( firstChangedIndex != lastChangedIndex)
//    	dataChanged( createIndex( firstChangedIndex, 0 ), createIndex(lastChangedIndex, columnCount()));
//    m_tag = tag;

}

void
PlaydarTagCloudModel::onTagError()
{

}

// virtual
QVariant
PlaydarTagCloudModel::data( const QModelIndex& index, int role ) const
{
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


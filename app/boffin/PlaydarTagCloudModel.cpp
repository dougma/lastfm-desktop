#include <float.h> 
#include <math.h>
#include "PlaydarTagCloudModel.h"
#include "PlaydarConnection.h"


PlaydarTagCloudModel::PlaydarTagCloudModel(PlaydarConnection *playdar)
:m_playdar(playdar)
{
}

PlaydarTagCloudModel::~PlaydarTagCloudModel()
{
}

void 
PlaydarTagCloudModel::startGetTags(const QString& rql)
{
    qRegisterMetaType<QList<BoffinTagItem> >("QList<BoffinTagItem>");

    BoffinTagRequest* req = m_playdar->boffinTagcloud(rql);
//    connect(req, SIGNAL(tags(QList<BoffinTagItem>)), this, SLOT(onTags(QList<BoffinTagItem>)));
// todo: ^
    connect(req, SIGNAL(error()), this, SLOT(onTagError()));
}

void 
PlaydarTagCloudModel::onTags(QList<BoffinTagItem> tags)
{
    typedef QMap<QString, float> TagWeightMap;
    typedef TagWeightMap::iterator TagWeightMapIterator;
    TagWeightMap tagWeightMap;

    foreach(const BoffinTagItem& i, tags) {
        if (!m_hostFilter.contains(i.m_host)) {
            // host is not being filtered.
            TagWeightMapIterator it = tagWeightMap.find(i.m_name);
            if (it == tagWeightMap.end()) {
                // first instance of this tag
                tagWeightMap.insert(i.m_name, i.m_weight);
            } else {
                // multiple hosts have this tag; add their weights:
                it.value() += i.m_weight;
            }
        }
    }

    m_tagHash.clear();
    m_logTagHash.clear();

    m_maxWeight = 0;
    m_minLogWeight = FLT_MAX;

    beginInsertRows( QModelIndex(), m_tagHash.size(), m_tagHash.size() + tagWeightMap.size());
    for (TagWeightMapIterator it = tagWeightMap.begin(); it != tagWeightMap.end(); it++)
    {
        float& weight = it.value();
        const QString& name = it.key();

        m_maxWeight = qMax( weight, m_maxWeight );
        float logWeight = log( weight );
        m_minLogWeight = qMin( logWeight, m_minLogWeight);
        m_logTagHash.insert( logWeight, name );
        m_tagHash.insert( weight, name );
    }
    endInsertRows();

    m_tags = tags;

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
            QMultiMap< float, QString >::const_iterator i = m_tagHash.constEnd();
            i -= index.row() + 1;
            return i.value();
        }
                
        case PlaydarTagCloudModel::WeightRole:
        {
            QMultiMap< float, QString>::const_iterator i = m_tagHash.constEnd();
            i -= index.row() + 1;
            return QVariant::fromValue<float>((i.key() / m_maxWeight));
        }

        case PlaydarTagCloudModel::LinearWeightRole:
        {
            QMultiMap< float, QString >::const_iterator i = m_logTagHash.constEnd();
            i -= index.row() + 1;
            return QVariant::fromValue<float>( ( i.key() - m_minLogWeight ) / ((m_logTagHash.constEnd() -1 ).key() - m_minLogWeight));
        }

        default:
            return QVariant();
    }
}


// virtual
QModelIndex 
PlaydarTagCloudModel::index( int row, int column, const QModelIndex& parent /*= QModelIndex()*/) const
{
    return parent.isValid() || row > m_tagHash.count() ? 
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

    return m_tagHash.count();
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
    onTags(m_tags);         // recalc things...
}

void
PlaydarTagCloudModel::addToHostFilter(const QString& hostname)
{
    m_hostFilter.insert(hostname);
    onTags(m_tags);
}

void 
PlaydarTagCloudModel::setTagMapping(QMap<QString, QString> tagMap)
{
    m_tagMap = tagMap;
    onTags(m_tags);
}


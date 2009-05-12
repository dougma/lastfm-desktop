#include "PlaydarTagCloudModel.h"
#include <float.h> 
#include <math.h>

PlaydarTagCloudModel::PlaydarTagCloudModel(PlaydarApi& p, lastfm::NetworkAccessManager* wam)
:m_api(p)
,m_wam(wam)
{
}

PlaydarTagCloudModel::~PlaydarTagCloudModel()
{
}

void 
PlaydarTagCloudModel::startGetTags(const QString& rql)
{
    qRegisterMetaType<QList<BoffinTagItem> >("QList<BoffinTagItem>");

    BoffinTagRequest* req = new BoffinTagRequest(m_wam, m_api, rql);
    connect(req, SIGNAL(tags(QList<BoffinTagItem>)), this, SLOT(onTags(QList<BoffinTagItem>)));
    connect(req, SIGNAL(error()), this, SLOT(onTagError()));
    req->start();
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

    m_tags = tags;

    reset();
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
    return parent.isValid() ? 
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
PlaydarTagCloudModel::rowCount( const QModelIndex& ) const
{
    return m_tagHash.count();
}

//virtual 
int 
PlaydarTagCloudModel::columnCount( const QModelIndex& ) const 
{ 
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


#include "XspfModel.h"

//virtual 
QModelIndex 
XspfModel::index(int row, int column, const QModelIndex &parent /* = QModelIndex() */) const
{
    if (parent.isValid()) {
        return createIndex(row, column, (int) parent.internalId());
    } else {
        return createIndex(row, column, -1);
    }
}

//virtual 
QModelIndex 
XspfModel::parent(const QModelIndex &child) const
{
    if (child.isValid() && child.internalId() >= 0) {
        return createIndex(child.internalId(), 0, -1);
    } else {
        return QModelIndex();
    }
}

//virtual 
int 
XspfModel::rowCount(const QModelIndex &parent /* = QModelIndex() */) const
{
    if (parent.isValid() && parent.internalId() >= 0) {
        return m_tracks[parent.internalId()].second.size();
    }
    return m_tracks.size();
}

//virtual 
int 
XspfModel::columnCount(const QModelIndex &parent /* = QModelIndex() */) const
{
    if (parent.isValid()) {
        // resolve result
        return 11;
    } else {
        // track
        return 5;
    }
}

//virtual 
QVariant 
XspfModel::data(const QModelIndex &index, int role /* = Qt::DisplayRole */) const
{
    if (index.isValid()) {
        if (index.internalId() >= 0) {
            BoffinPlayableItem i = m_tracks[index.internalId()].second[index.row()];
            switch (index.column()) {
                case 0: return i.artist();
                case 1: return i.album();
                case 2: return i.track();
                case 3: return i.score();
                case 4: return i.preference();
                case 5: return i.source();
                case 6: return i.bitrate();
                case 7: return i.duration();
                case 8: return i.size();
                case 9: return i.mimetype();
                case 10: return i.url();
            }
        } else {
            lastfm::Track t = m_tracks[index.row()].first;
            switch (index.column()) {
                case 0: return (QString)t.artist();
                case 1: return (QString)t.album();
                case 2: return t.title();
                case 3: return t.duration();
                case 4: return t.url();
            }
        }
    }
    return QVariant();
}

//
void
XspfModel::addTrack(const lastfm::Track& t)
{
    m_tracks.push_back(TrackResult(t, ResultList()));
}

void
XspfModel::addResult(int trackIndex, const BoffinPlayableItem& i)
{
    m_tracks[trackIndex].second.push_back(i);
}

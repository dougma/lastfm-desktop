#ifndef XSPF_MODEL_H
#define XSPF_MODEL_H

#include <QPair>
#include <QList>
#include <QAbstractItemModel>
#include "playdar/BoffinPlayableItem.h"
#include "lastfm/Track"

class XspfModel : public QAbstractItemModel
{
public:
    // the minimum QAbstractItemModel requirements:
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void addTrack(const lastfm::Track& t);
    void addResult(int trackIndex, const BoffinPlayableItem& i);

    typedef QList<BoffinPlayableItem> ResultList;
    typedef QPair<lastfm::Track, ResultList> TrackResult;

private:
    QList<TrackResult> m_tracks;
};

#endif


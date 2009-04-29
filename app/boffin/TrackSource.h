#ifndef TRACK_SOURCE_H
#define TRACK_SOURCE_H

#include <QList>
#include <lastfm/Track>
#include "BoffinRqlRequest.h"


class TrackSource 
    : public QObject
{
    Q_OBJECT

public:
    TrackSource(BoffinRqlRequest* req);    
    Track takeNextTrack();

signals:
    void ready();

private slots:
    void onTracks(QList<BoffinPlayableItem> tracks);

private:
    QList<BoffinPlayableItem> m_tracks;
};

#endif

#include "TrackSource.h"

TrackSource::TrackSource(BoffinRqlRequest* req)
{
    connect(req, SIGNAL(tracks(QList<BoffinPlayableItem>)), this, SLOT(onTracks(QList<BoffinPlayableItem>)));
}

static Track toTrack(const BoffinPlayableItem& item)
{
    Track t;
    MutableTrack mt(t);
    mt.setArtist(item.m_artist);
    mt.setAlbum(item.m_album);
    mt.setTitle(item.m_track);
    mt.setDuration(item.m_duration);
    mt.setUrl(QUrl(item.m_url));
    mt.setSource(Track::Player);
    //QString m_source;
    //QString m_mimetype;
    return t;
}

Track 
TrackSource::takeNextTrack()
{
    return m_tracks.isEmpty() ? Track() : toTrack(m_tracks.takeFirst());
}

void 
TrackSource::onTracks(QList<BoffinPlayableItem> tracks)
{
    m_tracks = tracks;
    emit ready();
}


#ifndef BOFFIN_RQL_REQUEST_H
#define BOFFIN_RQL_REQUEST_H

#include <lastfm/global.h>
#include "PlaydarApi.h"
#include "PlaydarPollingRequest.h"
#include "CometRequest.h"
struct BoffinPlayableItem
{
    BoffinPlayableItem(const QString& artist, 
        const QString& album, 
        const QString& track, 
        const QString& source, 
        const QString& mimetype, 
        const QString& url,
        int duration)
        : m_artist(artist)
        , m_album(album)
        , m_track(track)
        , m_source(source)
        , m_mimetype(mimetype)
        , m_url(QUrl::fromPercentEncoding(QByteArray(url.toUtf8())))
        , m_duration(duration)
    {
    }

    QString m_artist;
    QString m_album;
    QString m_track;
    QString m_source;
    QString m_mimetype;
    QUrl m_url;
    int m_duration;
};

class BoffinRqlRequest : public CometRequest
{
    Q_OBJECT

public:
    void issueRequest(lastfm::NetworkAccessManager* wam, PlaydarApi& api, const QString& rql, const QString& session);
    virtual void receiveResult(const QVariantMap& o);

signals:
    void error();
    void playableItem(BoffinPlayableItem item);
    void requestMade( const QString );

private slots:
    void onFinished();

private:
    void fail(const char* message);
};

#endif


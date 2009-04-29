#ifndef BOFFIN_RQL_REQUEST_H
#define BOFFIN_RQL_REQUEST_H

#include "PlaydarApi.h"
#include "PlaydarPollingRequest.h"


class QNetworkReply;
class WsAccessManager;


struct BoffinPlayableItem
{
    BoffinPlayableItem(const std::string& artist, 
        const std::string& album, 
        const std::string& track, 
        const std::string& source, 
        const std::string& mimetype, 
        const std::string& url,
        int duration)
        : m_artist(QString::fromStdString(artist))
        , m_album(QString::fromStdString(album))
        , m_track(QString::fromStdString(track))
        , m_source(QString::fromStdString(source))
        , m_mimetype(QString::fromStdString(mimetype))
        , m_url(QUrl::fromPercentEncoding(QByteArray(url.data())))
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



class BoffinRqlRequest
    : public QObject
    , public PlaydarPollingRequest
{
    Q_OBJECT
public:
    BoffinRqlRequest(WsAccessManager* wam, PlaydarApi& api, QString rql);
    ~BoffinRqlRequest();

signals:
    void error();
    void tracks(QList<BoffinPlayableItem> tracks);

private slots:
    void onReqFinished();
    void onPollFinished();
    void onTimer();

private:
    virtual void issueRequest();
    virtual void issuePoll(unsigned msDelay);
    virtual bool handleJsonPollResponse(int poll, const json_spirit::Object& query, const json_spirit::Array& results);
    virtual void fail(const char* message);

    PlaydarApi m_api;
    WsAccessManager *m_wam;
    QNetworkReply *m_rqlReply;
    QNetworkReply *m_pollReply;

    QString m_rql;
};

#endif


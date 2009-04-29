#ifndef BOFFIN_REQUEST_H
#define BOFFIN_REQUEST_H

#include "PlaydarApi.h"
#include "PlaydarPollingRequest.h"


class QNetworkReply;
class WsAccessManager;

struct BoffinTagItem
{
    BoffinTagItem(const std::string &name, const std::string &host, int count, float weight)
        : m_name(QString::fromStdString(name))
        , m_host(QString::fromStdString(host))
        , m_count(count)
        , m_weight(weight)
    {
    }

    QString m_name;
    QString m_host;
    int m_count;
    float m_weight;
};

class BoffinRequest 
    : public QObject
    , public PlaydarPollingRequest
{
    Q_OBJECT

public:
    BoffinRequest(WsAccessManager* wam, PlaydarApi& api);
    ~BoffinRequest();

signals:
    void error();
    void tags(QList<BoffinTagItem> tags);

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
    QNetworkReply *m_tagcloudReply;
    QNetworkReply *m_pollReply;
};

class StatRequest
{

};

#endif

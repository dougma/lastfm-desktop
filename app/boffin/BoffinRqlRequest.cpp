#include <QNetworkReply>
#include <QTimer>

#include <boost/foreach.hpp>
#include "json_spirit/json_spirit.h"
#include "lib/lastfm/ws/WsAccessManager.h"

#include "BoffinRqlRequest.h"
#include "jsonGetMember.hpp"


BoffinRqlRequest::BoffinRqlRequest(WsAccessManager* wam, PlaydarApi& api, QString rql)
: m_wam(wam)
, m_api(api)
, m_rqlReply(0)
, m_pollReply(0)
, m_rql(rql)
{
}

BoffinRqlRequest::~BoffinRqlRequest()
{
}

void 
BoffinRqlRequest::onReqFinished()
{
    QNetworkReply *reply = (QNetworkReply*) sender();
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray ba( reply->readAll() );
        handleResponse(ba.constData(), ba.size());
    }
    fail("");
}

void 
BoffinRqlRequest::onPollFinished()
{
    QNetworkReply *reply = (QNetworkReply*) sender();
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray ba( reply->readAll() );
        handlePollResponse(ba.constData(), ba.size());
    }
    fail("");
}

// virtual 
void 
BoffinRqlRequest::issueRequest()
{
    m_rqlReply = m_wam->get(QNetworkRequest(m_api.boffinRql(m_rql)));
    if (m_rqlReply) {
        connect(m_rqlReply, SIGNAL(finished()), this, SLOT(onReqFinished()));
    } else {
        fail("couldn't issue boffin rql request");
    }
}

// virtual 
void 
BoffinRqlRequest::issuePoll(unsigned msDelay)
{
    QTimer::singleShot(msDelay, this, SLOT(onTimer()));
}

//virtual 
bool 
BoffinRqlRequest::handleJsonPollResponse(int poll, const json_spirit::Object& query, const json_spirit::Array& results)
{
    query;

    QList<BoffinPlayableItem> trackList;
    BOOST_FOREACH(const json_spirit::Value& i, results) {
        int duration = 0;
        jsonGetMember(i, "duration", duration);

        std::string artist, album, track, source, mimetype, url;
        if (jsonGetMember(i, "artist", artist) &&
            jsonGetMember(i, "album", album) &&
            jsonGetMember(i, "track", track) &&
            jsonGetMember(i, "source", source) &&
            jsonGetMember(i, "mimetype", mimetype) &&
            jsonGetMember(i, "url", url) )
        {
            trackList << BoffinPlayableItem(artist, album, track, source, mimetype, url, duration);
        }
    }
    emit tracks(trackList);
    return poll < 4;
}

//virtual 
void 
BoffinRqlRequest::fail(const char* message)
{
    message;
    emit error();
}


void
BoffinRqlRequest::onTimer()
{
    // start the poll now
    delete m_pollReply; // free any previous

    m_pollReply = m_wam->get(QNetworkRequest(m_api.getResults(QString::fromStdString(qid()))));
    if (m_pollReply) {
        connect(m_pollReply, SIGNAL(finished()), this, SLOT(onPollFinished()));
    } else {
        fail("couldn't poll for boffin rql results");
    }
}
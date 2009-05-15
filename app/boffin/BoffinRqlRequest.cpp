#include <QNetworkReply>
#include <QTimer>

#include <boost/foreach.hpp>
#include "json_spirit/json_spirit.h"
#include <lastfm/NetworkAccessManager>

#include "BoffinRqlRequest.h"
#include "jsonGetMember.hpp"

// borken... sorry, in progress of coverting to comet

BoffinRqlRequest::BoffinRqlRequest(lastfm::NetworkAccessManager* wam, PlaydarApi& api, const QString& rql, const QString& session)
: m_wam(wam)
, m_api(api)
, m_rql(rql)
, m_session(session)
{
}

BoffinRqlRequest::~BoffinRqlRequest()
{
}

#if 0


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
BoffinRqlRequest::issueRequest()
{
    m_rqlReply = m_wam->get(QNetworkRequest(m_api.boffinRql(m_rql)));
    if (m_rqlReply) {
        connect(m_rqlReply, SIGNAL(finished()), this, SLOT(onReqFinished()));
    } else {
        fail("couldn't issue boffin rql request");
    }
}

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

#endif

#ifndef BOFFIN_RQL_REQUEST_H
#define BOFFIN_RQL_REQUEST_H

#include <lastfm/global.h>
#include "PlaydarApi.h"
#include "PlaydarPollingRequest.h"
#include "CometRequest.h"
#include "BoffinPlayableItem.h"

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


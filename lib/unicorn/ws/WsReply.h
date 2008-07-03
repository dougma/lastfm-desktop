#ifndef WS_RESPONSE_H
#define WS_RESPONSE_H

#include <QNetworkReply>
#include <QDomDocument>

class WsReply: public QNetworkReply
{
    public:
        QDomDocument domDocument();
};

#endif
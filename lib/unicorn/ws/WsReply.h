#ifndef WS_REPLY_H
#define WS_REPLY_H

#include <QNetworkReply>
#include <QDomDocument>
#include "lib/DllExportMacro.h"

class UNICORN_DLLEXPORT WsReply : public QNetworkReply
{
public:
    QDomDocument domDocument(); //can't be const
};

#endif
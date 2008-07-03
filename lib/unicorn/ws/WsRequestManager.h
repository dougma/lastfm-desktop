#ifndef WS_REQUEST_MANAGER_H
#define WS_REQUEST_MANAGER_H

#include "WsRequestParameters.h"
#include <QNetworkAccessManager>
#include <QMap>
#include "lib/DllExportMacro.h"

class UNICORN_DLLEXPORT WsRequestManager: QObject
{
    Q_OBJECT

public:
    static WsRequestManager *instance()
    { 
        if(!s_instance) s_instance = new WsRequestManager(); 
        return s_instance; 
    }

    class WsReply* getMethod( const QString methodName, WsRequestParameters& params )
    {
        return callMethod( methodName, params, GET );
    }

    class WsReply* syncGetMethod( const QString methodName, WsRequestParameters &params )
    {
        return syncCallMethod( methodName, params, GET );
    }

private:
    WsRequestManager(void);
    ~WsRequestManager(void);

    enum WsRequestType
    {
        GET = 0,
        POST
    };

    void getSession();

    static WsRequestManager* s_instance;
    bool m_hasHandshaken;
    QString m_baseHost;
    QString m_apiRoot;

    QNetworkAccessManager m_networkAccessManager;
    class WsReply* callMethod( const QString methodName, WsRequestParameters& params, int RequestType = GET );
    class WsReply* syncCallMethod( const QString methodName, WsRequestParameters &params, int RequestType = GET );


private slots:
    void parseSessionAuth( class WsReply* reply );

};

#endif //WS_REQUEST_MANAGER_H
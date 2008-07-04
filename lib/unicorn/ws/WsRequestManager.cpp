#include "WsRequestManager.h"
#include "WsReply.h"
#include "../UnicornCommon.h"
#include "../UnicornSettings.h"
#include <QXmlInputSource>
#include <QXmlReader>
#include <QEventLoop>
#include <QDebug>

using namespace Unicorn;

//static
    WsRequestManager* WsRequestManager::s_instance = 0;

WsRequestManager::WsRequestManager( void )
                 :m_hasHandshaken( false ),
                  m_baseHost( "http://ws.audioscrobbler.com/" ),
                  m_apiRoot( "2.0/" )
{
    Unicorn::Settings settings;

    if( settings.sessionKey().isEmpty() )
    {
        getSession();
    }
}

void WsRequestManager::getSession()
{
    Unicorn::Settings settings;

    WsRequestParameters params;
    QString authToken = md5( ( settings.username() + 
                               settings.password() ).toUtf8() );

    params.add( "username", settings.username() )
          .add( "authToken", authToken );
    
    WsReply* reply = syncCallMethod( "auth.getMobileSession", params );
    if( reply->error() != QNetworkReply::NoError )
    {
        //TODO: handle network error + early out
    }
    
    parseSessionAuth( reply );
}

WsRequestManager::~WsRequestManager( void )
{
}

WsReply* WsRequestManager::callMethod( const QString methodName, WsRequestParameters &params, int RequestType )
{
    params.add( "method", methodName );
    
    QUrl url( m_baseHost + m_apiRoot );
    url.setQueryItems( params );
    
    QNetworkRequest request( url );
    
    QNetworkReply* reply = 0;
    
    switch( RequestType )
    {
        case GET:
            reply = m_networkAccessManager.get( request );
            break;

        case POST:
            reply = m_networkAccessManager.post( request, url.encodedQuery() );
            break;

        default:
            Q_ASSERT( !"Unknown RequestType" );
            break;
    }
        
    WsReply* wsReply = static_cast< WsReply* >( reply );
    return wsReply;
}

WsReply* WsRequestManager::syncCallMethod( const QString methodName, WsRequestParameters &params, int RequestType )
{
    WsReply* reply = callMethod( methodName, params );
    QEventLoop eventLoop;

    connect( reply, SIGNAL( finished() ),
             &eventLoop, SLOT( quit() ) );
    
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
             &eventLoop, SLOT( quit() ) );

    eventLoop.exec();
    return reply;
}

void WsRequestManager::parseSessionAuth( WsReply* reply )
{
    Unicorn::Settings settings;
    Unicorn::MutableSettings mutableSettings;

    QDomDocument content = reply->domDocument();

    QDomElement lfm = content.firstChildElement( "lfm" );
    if( lfm.isNull() )
    {
        //TODO: error handling and early out
    }

    QString status = lfm.attribute( "status", "" );
    if( status != "ok" )
    {
        //TODO: error handling and early out
    }

    QDomElement session = lfm.firstChildElement( "session" );
    if( session.isNull() )
    {
        //TODO: error handling and early out
    }

    QDomElement key = session.firstChildElement( "key" );
    if( key.isNull() || !key.firstChild().isText() )
    {
        //TODO: error handling and early out
    }
    
    mutableSettings.setSessionKey( key.firstChild().nodeValue() );
    qDebug() << "AuthRequest Response:\n"  << content.toString();
    qDebug() << "Session Key: " << settings.sessionKey();
}

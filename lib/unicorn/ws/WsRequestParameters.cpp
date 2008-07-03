#include "WsRequestParameters.h"
#include "../UnicornCommon.h"

static const QString API_KEY = "c8c7b163b11f92ef2d33ba6cd3c2c3c3";
static const QString API_SECRET = "73582dfc9e556d307aead069af110ab8";

WsRequestParameters::WsRequestParameters( QObject* parent )
                    : QObject( parent )
{
}

WsRequestParameters::~WsRequestParameters(void)
{
}

WsRequestParameters::operator const QList< QPair< QString,QString > >()
{
    QList< QPair < QString, QString > > params;

    add( "api_key", API_KEY );
    add( "api_sig", methodSignature() );

    for( QMap< QString, QString >::iterator iter = m_paramList.begin();
         iter != m_paramList.end();
         iter++ )
    {
        params.push_back( QPair< QString, QString >( iter.key(), iter.value() ) );
    }

    return params;
}

WsRequestParameters& WsRequestParameters::add( const QString& key, const QString& value )
{
    m_paramList.insert( key, value );
    return *this;
}

QString WsRequestParameters::methodSignature()
{
    QString paramString;

    for( QMap<QString, QString>::iterator iter = m_paramList.begin();
         iter != m_paramList.end();
         iter++ )
    {
        paramString += iter.key() + iter.value();
    }

    paramString += API_SECRET;
    return Unicorn::md5( paramString.toUtf8() );
}
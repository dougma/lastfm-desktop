/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include "WsReply.h"
#include <QCoreApplication>
#include <QEventLoop>


WsReply::WsReply( QNetworkReply* r )
{
    r->setParent( this );

    m_error = Ws::NoError;
    m_reply = r;

    connect( m_reply, SIGNAL(finished()), SLOT(onFinished()) );
}


void
WsReply::finish()
{
    QEventLoop eventLoop;
    QObject::connect( m_reply, SIGNAL(finished()), &eventLoop, SLOT(quit()) );
    eventLoop.exec();
}


void
WsReply::onFinished()
{
    try 
    {
        m_data = m_reply->readAll();

        if (m_data.size())
        {
            m_xml.setContent( m_data );
            m_lfm = m_xml.documentElement();

            if (m_lfm.isNull())
                throw Ws::MalformedResponse;

            QString const status = m_lfm.attribute( "status" );
            QDomElement error = m_lfm.firstChildElement( "error" );
            uint const n = m_lfm.childNodes().count();

            if (status == "failed" || n == 1 && !error.isNull())
            {
                throw error.isNull()
                        ? Ws::MalformedResponse
                        : Ws::Error( error.attribute( "code" ).toUInt() );
            }

            if (n == 0) // nothing useful in the response
                throw Ws::MalformedResponse;
        }

        switch (m_reply->error())
        {
            case QNetworkReply::NoError:
                break;

            case QNetworkReply::RemoteHostClosedError:
            case QNetworkReply::ConnectionRefusedError:
            case QNetworkReply::TimeoutError:
            case QNetworkReply::SslHandshakeFailedError:
            case QNetworkReply::ContentAccessDenied:
            case QNetworkReply::ContentOperationNotPermittedError:
            case QNetworkReply::ContentNotFoundError:
            case QNetworkReply::AuthenticationRequiredError:
            case QNetworkReply::UnknownContentError:
            case QNetworkReply::ProtocolInvalidOperationError:
            case QNetworkReply::ProtocolFailure:
                qDebug() << networkErrorString( m_reply->error() );
                throw Ws::TryAgain;

            case QNetworkReply::HostNotFoundError:
            case QNetworkReply::UnknownNetworkError:
            case QNetworkReply::ProtocolUnknownError:
                throw Ws::UrLocalNetworkIsFuckedLol;

            case QNetworkReply::OperationCanceledError:
                throw Ws::Aborted;

            case QNetworkReply::ProxyConnectionRefusedError:
            case QNetworkReply::ProxyConnectionClosedError:
            case QNetworkReply::ProxyNotFoundError:
            case QNetworkReply::ProxyTimeoutError:
            case QNetworkReply::ProxyAuthenticationRequiredError:
            case QNetworkReply::UnknownProxyError:
                throw Ws::UrProxyIsFuckedLol;
        }

        m_error = Ws::NoError;
    }
    catch (Ws::Error e)
    {
        qWarning() << m_reply->url();
        qWarning() << m_xml.toString();
        m_error = e;

        switch (m_error)
        {
            case Ws::InvalidSessionKey:
                // NOTE will never be received during the LoginDialog stage
                // since that happens before this slot is registered with
                // QMetaObject in App::App(). Neat :)
                QMetaObject::invokeMethod( qApp, "onWsError", Q_ARG( Ws::Error, m_error ) );
                break;
        }
    }

    emit finished( this );

    // prevents memory leaks, but I don't like it anyway
    deleteLater();
}


QString 
WsReply::networkErrorString( QNetworkReply::NetworkError e )
{
    // I didn't translate these because, they are more useful to us, and they 
    // are easy to google. Hate me if you must --mxcl

    #define CASE( x ) case x: return #x;

    switch (e)
    {
        CASE( QNetworkReply::NoError )
        CASE( QNetworkReply::ConnectionRefusedError )
        CASE( QNetworkReply::RemoteHostClosedError )
        CASE( QNetworkReply::HostNotFoundError )
        CASE( QNetworkReply::TimeoutError )
        CASE( QNetworkReply::OperationCanceledError )
        CASE( QNetworkReply::SslHandshakeFailedError )
        CASE( QNetworkReply::ProxyConnectionRefusedError )
        CASE( QNetworkReply::ProxyConnectionClosedError )
        CASE( QNetworkReply::ProxyNotFoundError )
        CASE( QNetworkReply::ProxyTimeoutError )
        CASE( QNetworkReply::ProxyAuthenticationRequiredError )
        CASE( QNetworkReply::ContentAccessDenied )
        CASE( QNetworkReply::ContentOperationNotPermittedError )
        CASE( QNetworkReply::ContentNotFoundError )
        CASE( QNetworkReply::AuthenticationRequiredError )
        CASE( QNetworkReply::ProtocolUnknownError )
        CASE( QNetworkReply::ProtocolInvalidOperationError )
        CASE( QNetworkReply::UnknownNetworkError )
        CASE( QNetworkReply::UnknownProxyError )
        CASE( QNetworkReply::UnknownContentError )
        CASE( QNetworkReply::ProtocolFailure )
    
        default:
            return "Unknown error";
    }

    #undef CASE
}

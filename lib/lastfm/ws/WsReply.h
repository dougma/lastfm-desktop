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

#ifndef LASTFM_WS_REPLY_H
#define LASTFM_WS_REPLY_H

#include <lastfm/public.h>
#include <lastfm/core/CoreDomElement.h>
#include <lastfm/ws/WsError.h>
#include <QNetworkReply>
#include <QDomDocument>


/** @brief A Last.fm wrapper around a QNetworkReply
  * @author <max@last.fm>
  *
  * When you get this back from WsRequestBuilder the request has already been
  * sent. So connect to the finished signal before the event loop resumes!
  *
  * Create a slot in your derived QApplication object onWsError( Ws::Error )
  * to receive errors that require user interaction. If you don't your user
  * experience will suck :P
  */
class LASTFM_WS_DLLEXPORT WsReply : public QObject
{
    Q_OBJECT

    Ws::Error m_error;
    QNetworkReply* m_reply;
    QDomDocument m_xml;
    QDomElement m_lfm;
    QByteArray m_data;
    QVariant m_associatedData;

    friend class WsRequestBuilder;
	friend QDebug operator<<( QDebug, WsReply* );

    WsReply( QNetworkReply* );

public:
    /** the <lfm> element from the XML response, see http://last.fm/api */
    CoreDomElement lfm() const { return CoreDomElement( m_lfm ); }

    Ws::Error error() const { return m_error; }
    QNetworkReply::NetworkError networkError() const { return m_reply->error(); }
    static QString networkErrorString( QNetworkReply::NetworkError );

	QString method() const;

    /** expose some QNetworkReply methods of the same name: **/
    QVariant header( QNetworkRequest::KnownHeaders ) const;
    void abort();

    /* returns the value of the "Expires" response header, or an
     * invalid QDateTime for a missing or invalid Expires header */
    QDateTime expires() const;

    /** use this metadata component to remind you what wsreply this is */
    void setAssociatedData( const QVariant& v ) { m_associatedData = v; }
    QVariant associatedData() const { return m_associatedData; }

    QByteArray data() const { return m_data; }

	bool failed() const { return m_error != Ws::NoError; }

    /* this is a useful function, why doesn't Qt expose it? */
    static QDateTime fromHttpDate(const QByteArray &value);

signals:
    /** we call deleteLater() immediately after emitting this signal, so don't
      * store copies of the pointer */
    void finished( WsReply* );

private slots:
    void onFinished();
};


#include <QDebug>
inline QDebug operator<<( QDebug d, WsReply* r )
{
	return d << r->method() + ":" << "\n"
	         << r->m_reply->url() << "\n"
			 << r->data().trimmed();
}


inline QDebug operator<<( QDebug d, QNetworkReply::NetworkError e )
{
    return d << lastfm::qMetaEnumString<QNetworkReply>( e, "NetworkError" );
}

#endif

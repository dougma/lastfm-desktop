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

#ifndef WS_REPLY_H
#define WS_REPLY_H

#include "WsError.h"
#include "lib/core/EasyDomElement.h"
#include "lib/DllExportMacro.h"
#include <QNetworkReply>


/** @brief Essentially our QNetworkReply, hence the name 
  * @author <max@last.fm>
  *
  * When you get this back from WsRequestBuilder the request has already been
  * sent. So connect to the finished signal before the event loop resumes!
  *
  * Create a slot in your derived QApplication object onWsError( Ws::Error )
  * to receive errors that require user interaction. If you don't your user
  * experience will suck :P
  */
class WS_DLLEXPORT WsReply : public QObject
{
    Q_OBJECT

    Ws::Error m_error;
    QNetworkReply* m_reply;
    QDomDocument m_xml;
    QDomElement m_lfm;
    QByteArray m_data;

    friend class WsRequestBuilder;

    WsReply( QNetworkReply* );

public:
    /** the <lfm> element from the XML response, see http://last.fm/api */
    EasyDomElement lfm() const { return EasyDomElement( m_lfm ); }
    Ws::Error error() const { return m_error; }
    QNetworkReply::NetworkError networkError() const { return m_reply->error(); }
    static QString networkErrorString( QNetworkReply::NetworkError );

    /** blocks until complete
      * SERIOUSLY NEVER USE THIS APART FROM FOR EXPERIMENTATION!
      * It crashes like crazy!
      * As all sorts of things break when you start running your own event loop
      * sadly, especially don't chain WsReplys since they do deleteLater() */
//    void finish();

    QByteArray data() const { return m_data; }
	
	bool failed() const { return m_error != Ws::NoError; }

signals:
    /** we call deleteLater() immediately after emitting this signal, so don't
      * store copies of the pointer */
    void finished( WsReply* );

private slots:
    void onFinished();
};

#endif

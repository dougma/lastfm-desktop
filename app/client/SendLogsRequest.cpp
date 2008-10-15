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

#include "SendLogsRequest.h"
#include "WsKeys.h"
#include <QFile>
#include <QTextDocument>
#include <QDebug>


SendLogsRequest::SendLogsRequest( QString clientname, QString clientversion, QString usernotes )
{
    m_clientname = clientname;
    m_clientversion = clientversion;
    m_usernotes = usernotes;
}


void
SendLogsRequest::addLog( QString name, QString filename )
{
    if ( QFile::exists( filename ) )
    {
        QFile file( filename );
        if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
            return;
    
        m_data.append( postData( name, qCompress( escapeString( file.readAll() ).toLatin1() ) ) );
        m_logs.append( name );
    }
}


void
SendLogsRequest::addLogData( QString name, QString data )
{
    m_data.append( postData( name, qCompress( escapeString( data ).toLatin1() ) ) );
    m_logs.append( name );
}

 
void
SendLogsRequest::send()
{
    QString url = "http://oops.last.fm/logsubmission/add";
    
    url += "?username=" + QString(Ws::Username);
    #ifdef Q_WS_MAC
        url += "&platform=macosx";
    #elif defined WIN32
        url += "&platform=win32";
    #else
        url += "&platform=linux";
    #endif
    url += "&clientname=" + m_clientname;
    url += "&clientversion=" + m_clientversion;

    QNetworkRequest request = QNetworkRequest( QUrl( url ) );
    request.setRawHeader( "Host", "oops.last.fm" );
    request.setRawHeader( "Content-Type", "multipart/form-data;"
                                          "boundary=8e61d618ca16" );
    request.setRawHeader( "Accept", "*/*" );

    m_data.append( postData( "usernotes", escapeString( m_usernotes ).toLatin1() ) );
    m_data.append( postData( "logs", m_logs.join(",").toLatin1() ) );
    m_data.append( "--8e61d618ca16--" ); // close the post request
    
    m_error = false;
    QNetworkReply *reply = (new WsAccessManager( this ))->post( request, m_data );
    
    connect( reply, SIGNAL( finished() ),
             this, SLOT( onFinished() ) );
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
             this, SLOT( onError( QNetworkReply::NetworkError ) ) );
}


QString
SendLogsRequest::escapeString( QString str )
{
    QString escaped_string;
    QString tmp_string = Qt::escape( str ); // Only escapes & < > and some more
    
    for (int i = 0; i < tmp_string.size(); ++i)
    {
         if ( tmp_string.at(i) > QChar('~')/*character 127*/ )
             escaped_string += "&#" + QString::number( tmp_string.at(i).unicode() ) + ";";
         else
            escaped_string += tmp_string.at(i);
    }
    
    return escaped_string;
}


QByteArray
SendLogsRequest::postData( QString name, QByteArray data )
{
    QByteArray postdata;

    postdata.append( "--8e61d618ca16\r\n" );
    postdata.append( "Content-Disposition: " );
    postdata.append( "form-data; name=\"" + name + "\"" );
    postdata.append( "\r\n\r\n" );

    postdata.append( data );

    postdata.append( "\r\n");
    
    return postdata;
}


void
SendLogsRequest::onFinished()
{
    if (!m_error)
        emit success();
    delete this;
    qDebug() << "Sent SendLogsRequest successfully";
}


void
SendLogsRequest::onError( QNetworkReply::NetworkError code )
{
    m_error = true;
    emit error();
}


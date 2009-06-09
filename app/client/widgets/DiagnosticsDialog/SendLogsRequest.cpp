/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "SendLogsRequest.h"
#include <lastfm/WsKeys>
#include <QCoreApplication>
#include <QFile>
#include <QTextDocument>
#include <QDebug>


SendLogsRequest::SendLogsRequest( const QString& usernotes )
{
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
    QUrl url;
    url.setScheme( "http" );
    url.setHost( "oops.last.fm" );
    url.setPath( "logsubmission/add" );
    url.addEncodedQueryItem( "username", QUrl::toPercentEncoding(Ws::Username) );
    #ifdef Q_WS_MAC
        url.addQueryItem( "platform", "macosx" );
    #elif defined WIN32
        url.addQueryItem( "platform", "win32" );
    #else
        url.addQueryItem( "platform", "linux" );
    #endif
    url.addQueryItem( "clientname", QCoreApplication::applicationName() );
    url.addQueryItem( "clientversion", QCoreApplication::applicationVersion() );

    QNetworkRequest request = QNetworkRequest( url );
    request.setRawHeader( "Content-Type", "multipart/form-data;boundary=8e61d618ca16" );
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
SendLogsRequest::onError( QNetworkReply::NetworkError )
{
    m_error = true;
    emit error();
}


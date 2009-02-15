/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
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

#include "FirehoseModel.h"
#include "app/moose.h"
#include "lib/lastfm/ws/WsDomElement.h"
#include <QLocale>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTcpSocket>

#define FIREHOSE_HOST "firehose.last.fm"


FirehoseModel::FirehoseModel()
             : m_socket( 0 )
             , m_cumulative_count( 0 )
{}


void
FirehoseModel::setNozzle( const QString& nozzle )
{
    m_tracks.clear();
    m_avatars.clear();
    m_users.clear();
    reset();

    m_nozzle = nozzle;
    
    reconnect();
}


void
FirehoseModel::reconnect()
{
    if (m_nozzle.isEmpty()) return;

    // using WsAccessManager stopped after the HEADERS were returned
    // for some reason. We block signals to prevent recursively calling this 
    // function
    if (m_socket) m_socket->blockSignals( true );
    delete m_socket;
    m_socket = new QTcpSocket( this );
    connect( m_socket, SIGNAL(connected()), SLOT(onConnect()) );
    connect( m_socket, SIGNAL(readyRead()), SLOT(onData()) );
    connect( m_socket, SIGNAL(disconnected()), SLOT(onFinished()) );
    m_socket->connectToHost( FIREHOSE_HOST, 80 );
}


void
FirehoseModel::onConnect()
{
    m_socket->write( "GET /stream/" + m_nozzle.toUtf8() + "\r\n"
                     "Host: " FIREHOSE_HOST "\r\n"
                     "Connection: keep-alive\r\n\r\n" );
    m_socket->flush();
}


void
FirehoseModel::onData()
{    
    QTcpSocket* r = (QTcpSocket*)sender();
    QByteArray data = r->readAll();
    QDomDocument xml;
    xml.setContent( data );
    
    // I'm mainly lazy because Track.operator== won't work here */
    static QByteArray mxcl_is_lazy; 
    if (data == mxcl_is_lazy)
        return;
    mxcl_is_lazy = data;
    
    try
    {
        WsDomElement e( xml.documentElement() );
        connect( new FirehoseItem( e ), SIGNAL(finished( FirehoseItem* )), SLOT(onItemReady( FirehoseItem* )) );
    }
    catch (WsDomElement::Exception& e)
    {
        qWarning() << e;
        qDebug() << data;
    }
}

#include <QtNetwork/QHttp>
FirehoseItem::FirehoseItem( const WsDomElement& e )
            : m_user( e["user"]["name"].text() )
{	
    m_track = e["track"]["artist"]["name"].text() + " - " + e["track"]["name"].text();

	QUrl url( e["user"]["image"].text() );

	QHttp* http = new QHttp( url.host(), 80, this );
	m_id = http->get( url.path() );
    
    connect( http, SIGNAL(requestFinished( int, bool )), SLOT(onAvatarDownloaded( int )) );
}


void
FirehoseItem::onAvatarDownloaded( int id )
{
	if (id != m_id) return;
	
    QByteArray data = static_cast<QHttp*>(sender())->readAll();
    
    m_avatar.loadFromData( data );
    if (m_avatar.isNull())
        m_avatar = QPixmap( ":/lastfm/no/avatar.png" );
    emit finished( this );
}


void
FirehoseModel::onItemReady( FirehoseItem* item )
{   
    beginInsertRows( QModelIndex(), 0, 0 );
    m_tracks.prepend( item->track() );
    m_avatars.prepend( item->avatar() );
    m_users.prepend( item->user() );
    m_timestamps.prepend( QDateTime::currentDateTime() );
    m_cumulative_count++;
    endInsertRows();
    item->deleteLater();
}


void
FirehoseModel::onFinished()
{
    qDebug() << "Firehose disconnected. Reconnecting.";
    reconnect();
}


QVariant
FirehoseModel::data(const QModelIndex &index, int role) const
{   
    int const row = index.row();
    
    if (!index.isValid()) return QVariant();
    if (row < 0 || row >= m_users.count()) return QVariant();
    
    switch (role)
    {
        case Qt::DisplayRole: return m_users[row];
        case Qt::DecorationRole: return m_avatars[row];
        case moose::TrackRole: return QVariant::fromValue( m_tracks[row] );
        case moose::TimestampRole: return m_timestamps[row];
        case moose::CumulativeCountRole: return m_cumulative_count;
            
        case moose::SecondaryDisplayRole: return m_tracks[row];
        case moose::SmallDisplayRole:
        {
            QString format = QLocale().timeFormat( QLocale::ShortFormat );
            return m_timestamps[row].toString( format );
        }
    }
    
    return QVariant();
}


void
FirehoseModel::prune()
{
    if (m_tracks.count() <= 20)
        return;
    
    beginRemoveRows( QModelIndex(), 20, m_tracks.count() - 1 );
    while (m_tracks.count() > 20)
    {
        m_tracks.pop_back();
        m_avatars.pop_back();
        m_users.pop_back();
        m_timestamps.pop_back();
    }
    endRemoveRows();
}

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

#include "FirehoseModel.h"
#include "app/moose.h"
#include "lib/lastfm/core/CoreDomElement.h"
#include "lib/lastfm/ws/WsAccessManager.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTcpSocket>


FirehoseModel::FirehoseModel()
             : m_socket( 0 ), m_cumulative_count( 0 )
{}


void
FirehoseModel::setNozzle( const QString& nozzle )
{
    m_tracks.clear();
    m_avatars.clear();
    m_users.clear();
    reset();
    
    delete m_socket;
    
    // using a socket as WsAccessManager stopped after the HEADERS were returned
    // for some reason
    m_socket = new QTcpSocket( this );
    m_socket->connectToHost( "firehose.last.fm", 80 );
    m_socket->waitForConnected( 1000 ); //FIXME
    
    QByteArray headers = 
    "GET /stream/"+nozzle.toUtf8()+"\r\n"
    "Host: firehose.last.fm\r\n"
    "Connection: keep-alive\r\n\r\n";
    
    m_socket->write( headers );
    m_socket->flush();
    
    connect( m_socket, SIGNAL(readyRead()), SLOT(onData()) );
    connect( m_socket, SIGNAL(aboutToClose()), SLOT(onFinished()) );    
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
        CoreDomElement e( xml.documentElement() );
        connect( new FirehoseItem( e ), SIGNAL(finished( FirehoseItem* )), SLOT(onItemReady( FirehoseItem* )) );
    }
    catch (CoreDomElement::Exception& e)
    {
        qWarning() << e;
        qDebug() << data;
    }
}


FirehoseItem::FirehoseItem( const CoreDomElement& e )
            : m_user( e["user"]["name"].text() )
{
    static WsAccessManager* nam = 0;
    if (!nam) nam = new WsAccessManager;
    
    MutableTrack t;
    t.setArtist( e["track"]["artist"]["name"].text() );
    t.setTitle( e["track"]["name"].text() );
    m_track = t;
    
    QNetworkReply* r = nam->get( QNetworkRequest( e["user"]["image"].text() ) );
    
    connect( r, SIGNAL(finished()), SLOT(onAvatarDownloaded()) );
}


void
FirehoseItem::onAvatarDownloaded()
{
    QByteArray data = static_cast<QNetworkReply*>(sender())->readAll();
    
    m_avatar.loadFromData( data );
    if (m_avatar.isNull())
        m_avatar = QPixmap( ":/lastfm/no/user.png" );
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
    delete item;
}


void
FirehoseModel::onFinished()
{
    qDebug() << "OHAI! We d/c'd. I guess you put your laptop to sleep or sumfink?";
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
            
        case moose::SecondaryDisplayRole: return m_tracks[row].toString();
        case moose::SmallDisplayRole:
        {
            QString format = CoreLocale::system().qlocale().timeFormat( QLocale::ShortFormat );
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

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

#include "Firehose.h"
#include "FirehoseDelegate.h"
#include "widgets/UnicornWidget.h"
#include "widgets/UnicornTabWidget.h"
#include "lib/core/CoreDomElement.h"
#include <QDebug>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QWebView>
#include <QVBoxLayout>

#include <QtGui>

#include "lib/ws/WsAccessManager.h"
WsAccessManager* nam = 0;


Firehose::Firehose()
{
    if (!nam) nam = new WsAccessManager;
    
    setWindowTitle( tr("Scrobbling Now") );
    
    Unicorn::TabBar* tabs;
    QListView* view;
    
    QVBoxLayout *v = new QVBoxLayout( this );
    v->addWidget( tabs = new Unicorn::TabBar );
    v->addWidget( view = new QListView );
    v->setMargin( 0 );
    v->setSpacing( 0 );
        
    view->setModel( new FirehoseModel );
    delete view->itemDelegate();
    view->setItemDelegate( new FirehoseDelegate );
    view->model()->setParent( this );
    view->itemDelegate()->setParent( this );
    view->setAttribute( Qt::WA_MacShowFocusRect, false );

    CoreSignalMapper* mapper = new CoreSignalMapper( this );
    mapper->setMapping( 0, "user/1000002?rt=xml&special=staffmembers" );
    mapper->setMapping( 1, "user/2113030?rt=xml" );
    connect( tabs, SIGNAL(currentChanged( int )), mapper, SLOT(map( int )) );
    connect( mapper, SIGNAL(mapped( QString )), view->model(), SLOT(setNozzle( QString )) );
    
    tabs->addTab( tr("Last.fm Staff") );
    tabs->addTab( tr("All your Friends") );    
    
    UnicornWidget::paintItBlack( this );
    QPalette p = palette();
    p.setBrush( QPalette::Base, p.window() );
    p.setBrush( QPalette::Text, p.windowText() );
    setPalette( p );
    view->setPalette( palette() );
    view->setAutoFillBackground( true );
    
    tabs->succombToTheDarkSide();
}



QSize
Firehose::sizeHint() const
{
    return QSize( 358, 600 );
}


#include <QTcpSocket>
FirehoseModel::FirehoseModel()
             : m_socket( 0 )
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
    QNetworkReply* r = (QNetworkReply*)sender();
    QByteArray data = r->readAll();
    QDomDocument xml;
    xml.setContent( data );

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
    int n = m_tracks.count();
    
    if (n-- > 20)
    {
        beginRemoveRows( QModelIndex(), n, n );
        m_tracks.pop_back();
        m_avatars.pop_back();
        m_users.pop_back();
        endRemoveRows();
    }
    
    beginInsertRows( QModelIndex(), 0, 0 );

    m_tracks.prepend( item->track() );
    m_avatars.prepend( item->avatar() );
    m_users.prepend( item->user() );

    endInsertRows();
}


void
FirehoseModel::onFinished()
{
    qDebug() << "Oh, we d/c'd. Prolly an error happened";
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
        case TrackRole: return QVariant::fromValue( m_tracks[row] );
    }
    
    return QVariant();
}

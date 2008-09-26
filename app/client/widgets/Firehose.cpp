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
#include "widgets/UnicornWidget.h"
#include "widgets/UnicornTabWidget.h"
#include "lib/core/CoreDomElement.h"
#include "lib/ws/WsAccessManager.h"
#include <QDebug>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QWebView>
#include <QVBoxLayout>

#include <QtGui>

#include "lib/ws/WsAccessManager.h"
WsAccessManager* nam = 0;


#include <QPainter>
class FirehoseDelegate : public QAbstractItemDelegate
{
    static int& metric()
    {
        static int metric;
        return metric;
    }
    
    virtual void paint ( QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
    {
        bool const isSelected = option.state & QStyle::State_Selected;
        bool const isActive = option.state & QStyle::State_Active;
        
        painter->save();
        
        if (isSelected)
        {
            QPalette::ColorGroup cg = option.state & QStyle::State_Enabled
                    ? QPalette::Normal
                    : QPalette::Disabled;
            
            if (cg == QPalette::Normal && !isActive)
                cg = QPalette::Inactive;
            
            painter->fillRect( option.rect, option.palette.brush(cg, QPalette::Highlight));
        } 
        else if (index.row() % 2 != index.model()->rowCount() % 2)
        {
            painter->fillRect(option.rect, option.palette.brush( QPalette::AlternateBase ) );
        }
        
        QTransform t;
        QPoint p = option.rect.topLeft();
        t.translate( p.x(), p.y() );
        painter->setTransform( t );
        
        const int S = painter->fontMetrics().lineSpacing();

        QPixmap px = index.data( Qt::DecorationRole ).value<QPixmap>();
        const int n = option.rect.height() - 20;
        px = px.scaled( n, n );
        painter->drawPixmap( 10, 10, px );
        
        // keep text within the rects we drew
        painter->setClipRect( 5, 5, option.rect.width() - 10, option.rect.height() - 10 );
        
        QColor primary = isSelected ? (isActive ? Qt::white : Qt::darkGray) : Qt::white;
        QColor secondary = isSelected && isActive ? Qt::white : Qt::darkGray; 
        
        painter->setPen( secondary );
        painter->drawText( metric() , 5 + S + 2 + S, index.data( TrackRole ).value<Track>().toString() );

        QFont f = painter->font();
        f.setBold( true );
        painter->setFont( f );
        painter->setPen( primary );
        painter->drawText( metric() , 5 + S, index.data().toString() );
        
        painter->restore();
    }
    
    virtual QSize sizeHint( const QStyleOptionViewItem& o, const QModelIndex& ) const
    {
        if (metric() == 0) metric() = 1 /*top margin*/ + QFontMetrics( o.font ).lineSpacing() * 3 + 2 /*text spacing*/ + 4 /*bottom margin*/;
        return QSize( metric() + 150, metric() );
    }
};



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
    
    tabs->addTab( tr("Last.fm Staff") );
    tabs->addTab( tr("All your Friends") );
    
    view->setModel( new FirehoseModel );
    delete view->itemDelegate();
    view->setItemDelegate( new FirehoseDelegate );
    view->model()->setParent( this );
    view->itemDelegate()->setParent( this );
    view->setAttribute( Qt::WA_MacShowFocusRect, false );

    // mxcl's awesome protected hack (tm)
    struct ProtectionHack : QAbstractScrollArea { using QAbstractScrollArea::setViewportMargins; };
    reinterpret_cast<ProtectionHack*>(view)->setViewportMargins( 4, 4, 4, 4 );

    UnicornWidget::paintItBlack( this );
    
    QLinearGradient g( QPointF(), QPointF( 0, 656 ) );
    g.setColorAt( 0, Qt::black );
    g.setColorAt( 1, QColor( 25, 24, 24 ) );

    QPalette p = palette();    
    p.setBrush( QPalette::Base, g );
    p.setBrush( QPalette::Text, Qt::white );
    
    view->setPalette( p );
    view->setAutoFillBackground( true );
}



QSize
Firehose::sizeHint() const
{
    return QSize( 358, 600 );
}


#include <QTcpSocket>
FirehoseModel::FirehoseModel()
{
    // using a socket as WsAccessManager stopped after the HEADERS were returned
    // for some reason
    QTcpSocket* socket = new QTcpSocket( this );
    socket->connectToHost( "87.117.229.70", 8001 );
    socket->waitForConnected( 1000 );
    
    QByteArray headers = 
    "GET /firehose/user/1000002?rt=xml&special=staffmembers\r\n"
    "Host: 87.117.229.70\r\n"
    "Connection: keep-alive\r\n\r\n";
    
    socket->write( headers );
    socket->flush();

#if 0
    // set to RJ's user id for the moment
    QUrl url( "http://87.117.229.70/firehose/artist/979?rt=xml" );
    QNetworkRequest request( url );
    
    QNetworkReply* r = m_nam->get( request );
#endif
    QTcpSocket* r = socket;
    connect( r, SIGNAL(readyRead()), SLOT(onData()) );
    connect( r, SIGNAL(aboutToClose()), SLOT(onFinished()) );
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
    
    if (n > 20)
    {
        --n;
        beginRemoveRows( QModelIndex(), n, n );
        m_tracks.pop_back();
        m_avatars.pop_back();
        m_users.pop_back();
        endRemoveRows();
        n--;
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

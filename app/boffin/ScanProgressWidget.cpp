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

#include "ScanProgressWidget.h"
#include "lib/lastfm/types/Track.h"
#include <QtGui>
#include <QtNetwork>


ScanProgressWidget::ScanProgressWidget()
{
//    QObject* o = new ScanProgressMock;
//    connect( o, SIGNAL(track( Track )), SLOT(onNewTrack( Track )) );
    
    (new QBasicTimer)->start( 20, this );
    
    setBackgroundRole( QPalette::Base );
    setAutoFillBackground( true );
    setAttribute( Qt::WA_MacShowFocusRect, false );
    setAttribute( Qt::WA_MacSmallSize );
}


void
ScanProgressWidget::onNewTrack( const Track& t )
{
    int& i = count( t.artist() );
    i++;
    tracks.prepend( t );
    
    // so this is a time saving way to keep the list the size of the screen
    // it goes over a bit almost certainly, but it's ok
    if (tracks.size() > 60)
        tracks.pop_back();
    
    if (i == 1)
    {
        QObject* o = new ImageFucker( t.artist() );
        connect( o, SIGNAL(fucked()), SLOT(onImageFucked()) );
        o->setParent( this );
    }
}


void
ScanProgressWidget::paintEvent( QPaintEvent* e )
{
    {
        QPainter p( this );
        p.setPen( Qt::lightGray );
        int i = 15;
        foreach (Track track, tracks)
        {
            p.drawText( 6, i, track.url().path() );
            i += 18;
        }
    }
    
    for (int i = 0; i < images.count(); ++i)
    {
        QPainter p( this );
        p.setRenderHint( QPainter::Antialiasing );
        p.setRenderHint( QPainter::SmoothPixmapTransform );

        int y = (images[i]->y * (height() - images[i]->pixmap.height())) / 1000;

        QPointF pt( width() - images[i]->x, y );

        p.setOpacity( images[i]->opacity );
        p.drawImage( pt, images[i]->pixmap );

        QString text = images[i]->artist + "\n" + QString( "%L1 tracks" ).arg( count( images[i]->artist ) );

        QRectF rect( pt.x(), y + (images[i]->pixmap.height() * 0.75), images[i]->pixmap.width(), height() );

        p.setPen( Qt::black );
        p.drawText( rect, Qt::AlignTop | Qt::AlignHCenter, text );
    }
}


void
ScanProgressWidget::timerEvent( QTimerEvent* )
{
    for (int i = 0; i < images.count(); ++i)
    {
        images[i]->opacity += 0.5f / images[i]->pixmap.width();
        images[i]->x += 0.5;
    }
    update();
}


void
ScanProgressWidget::onImageFucked()
{
    images += (ImageFucker*)sender();
}


void
ImageFucker::onArtistGotInfo( WsReply* wsreply )
{
    QUrl url = Artist::getInfo( wsreply ).imageUrl();
    QNetworkReply* reply = nam.get( QNetworkRequest(url) );
    connect( reply, SIGNAL(finished()), SLOT(onImageDownloaded()) );
}


static inline QImage reflect0rize( const QImage& in )
{
    const int H = in.height() / 3;
    
    QImage r = QImage( in.width(), in.height() + H, QImage::Format_ARGB32_Premultiplied );
    
	QImage in2 = in;
	in2.convertToFormat( QImage::Format_ARGB32_Premultiplied );
    
    QImage reflection = in.copy( 0, in.height() - H, in.width(), H );
    reflection = reflection.mirrored( false, true /*vertical only*/ );
    
    QPainter p( &r );
    p.drawImage( 0, 0, in2 );
    p.drawImage( 0, in.height(), reflection );

    QLinearGradient g( QPointF( 0, 0 ), QPointF( 0, 1 ) );
    g.setCoordinateMode( QGradient::ObjectBoundingMode );
    g.setColorAt( 0, QColor(0, 0, 0, 100) );
    g.setColorAt( 1, Qt::transparent );

    p.setRenderHint( QPainter::Antialiasing );
    p.setRenderHint( QPainter::SmoothPixmapTransform );

    p.setCompositionMode( QPainter::CompositionMode_DestinationIn );
    p.fillRect( QRectF( QPointF( 0, in.height() ), QSizeF( r.width(), H ) ), g );
    return r;
}


void
ImageFucker::onImageDownloaded()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
    QByteArray const data = reply->readAll();
    
    QImage in;
    in.loadFromData( data );
    
    height = in.height();
    pixmap = reflect0rize( in );
    
    y = rand() % 1000;
    
    emit fucked();
}

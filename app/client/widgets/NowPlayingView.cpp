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

#include "NowPlayingView.h"
#include "ObservedTrack.h"
#include "PlayerEvent.h"
#include <QtGui>


static inline QImage compose( const QImage &in )
{
    qDebug() << "Image dimensions:" << in.size();

    const uint H = qreal(in.height()) / 2;

    QImage out( in.width(), in.height() + H, QImage::Format_ARGB32_Premultiplied );
    QPainter p( &out );
    p.drawImage( 0, 0, in );
    
    QImage reflection = in.copy( 0, in.height() - H, in.width(), H );
    reflection = reflection.mirrored( false, true /*vertical only*/ );

    QLinearGradient gradient( QPoint( 0, in.height() ), QPoint( 0, out.height() ) );
    gradient.setColorAt( 0, QColor(0, 0, 0, 100) );
    gradient.setColorAt( 1, Qt::transparent );

    p.drawImage( 0, in.height(), reflection );
    p.setCompositionMode( QPainter::CompositionMode_DestinationIn );
    p.fillRect( QRect( QPoint( 0, in.height() ), reflection.size() ), gradient );

    return out;
}



NowPlayingView::NowPlayingView( QWidget* parent )
              : QWidget( parent )
{
    //setMinimumSize( 150, 225 );
    setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );

    connect( qApp, SIGNAL(event( int, QVariant )), SLOT(onAppEvent( int, QVariant )) );

    QVBoxLayout* v = new QVBoxLayout( this );
    v->addStretch();
    v->addWidget( m_label = new QLabel );
    v->addSpacing( 8 );
    
    m_label->setAlignment( Qt::AlignBottom | Qt::AlignHCenter );
    m_label->setTextFormat( Qt::RichText );
}


void
NowPlayingView::onAppEvent( int e, const QVariant& v )
{
    switch (e)
    {
        case PlayerEvent::PlaybackEnded:
            m_cover = QImage();
            m_label->clear();
            update();
            break;

        case PlayerEvent::PlaybackStarted:
        case PlayerEvent::TrackChanged:
        {
            Track t = v.value<ObservedTrack>();
            QByteArray const data = t.album().image();
            if (data.size()) {
                m_cover.loadFromData( data );
                m_cover = m_cover.convertToFormat( QImage::Format_ARGB32_Premultiplied );
                m_cover = compose( m_cover );
            }
            else
                m_cover = QImage();

            //TODO album/artist/track may be empty
            m_label->setText( t.artist() + "<br><b>" + t.title() + "</b><br>" + t.album() );
            update();
            break;
        }
    }
}


void
NowPlayingView::paintEvent( QPaintEvent* e )
{
#if 0
    QPainter p( this );
    p.setClipRect( e->rect() );
    p.setRenderHint( QPainter::Antialiasing );
    p.setRenderHint( QPainter::SmoothPixmapTransform );

    QLinearGradient g( QPoint(), QPoint( width(), height() ) );
    g.setColorAt( 0, Qt::transparent );
    g.setColorAt( 1, QColor( 0x2b, 0x2b, 0x2b ) );
    p.fillRect( rect(), g );

    if (m_cover.isNull()) return;

    // determine rotated height
    QTransform trans;
    trans.rotate( -27, Qt::YAxis );
    QRectF r1 = rect().translated( -width()/2, -height()/2 );
    qreal const h = trans.inverted().map( QLineF( r1.topLeft(), r1.bottomLeft() ) ).dy();

    // calculate scaling factor
    qreal const scale = h / m_cover.height();
    trans.scale( scale, scale );

    // draw
    p.setTransform( trans * QTransform().translate( height()/2, height()/3 + 10 ) );
    p.drawImage( QPoint( -m_cover.height()/2, -m_cover.height()/3 ), m_cover );
#endif

    if (m_cover.isNull()) return;

    QPainter p( this );
    p.setClipRect( e->rect() );
    p.setRenderHint( QPainter::Antialiasing );
    p.setRenderHint( QPainter::SmoothPixmapTransform );
    QTransform trans;
    qreal const scale = qreal(height()) / m_cover.height();
    trans.scale( scale, scale );
    p.setTransform( trans );

    QPointF f = trans.inverted().map( QPointF( width(), 0 ) );

    f.rx() -= m_cover.width();
    f.rx() /= 2;

    p.drawImage( f, m_cover );
}

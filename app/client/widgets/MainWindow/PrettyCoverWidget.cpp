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
#include "PrettyCoverWidget.h"
#include <QSvgRenderer>
#include <QtGui>


PrettyCoverWidget::PrettyCoverWidget()
                 : m_reflection_height( 30 )
{}


void
PrettyCoverWidget::clear()
{
    m_cover = QImage();
    update();
}


void
PrettyCoverWidget::setImage( const QImage& in )
{
    const uint H = in.height();
	
    m_cover = QImage( in.width(), in.height() + H, QImage::Format_ARGB32_Premultiplied );
    
	QImage in2 = in;
	in2.convertToFormat( QImage::Format_ARGB32_Premultiplied );
    
    QImage reflection = in.copy( 0, in.height() - H, in.width(), H );
    reflection = reflection.mirrored( false, true /*vertical only*/ );
    
    QPainter p( &m_cover );
    p.drawImage( 0, 0, in2 );
    p.drawImage( 0, in.height(), reflection );
    p.end();

    update();
}


static inline int notZero( int i ) { return i ? i : 1; }


void
PrettyCoverWidget::paintEvent( QPaintEvent* )
{
    if (m_cover.isNull())
        return;
    
    qreal const h = qreal(m_cover.height()) / 2;
    qreal const f = h / notZero( height() - m_reflection_height );
    qreal const h_reflection = f * m_reflection_height;
    
    QImage composition = m_cover;

    QPainter p( &composition );
    p.setRenderHint( QPainter::Antialiasing );
    p.setRenderHint( QPainter::SmoothPixmapTransform );

    QLinearGradient g( QPointF( 0, 0 ), QPointF( 0, 1 ) );
    g.setCoordinateMode( QGradient::ObjectBoundingMode );
    g.setColorAt( 0, QColor(0, 0, 0, 100) );
    g.setColorAt( 1, Qt::transparent );

    p.setCompositionMode( QPainter::CompositionMode_DestinationIn );
    p.fillRect( QRectF( QPointF( 0, h ), QSizeF( m_cover.width(), h_reflection ) ), g );
    p.end();

    //prevent single pixel errors at the base of the image
    composition = composition.copy( 0, 0, composition.width(), h + h_reflection );

//////
    QTransform transform;
    qreal const scale = qreal(1) / f;
    transform.scale( scale, scale );

    QPointF point = QPointF( f, 0 );
    point.rx() *= width();
    point.rx() -= m_cover.width();
    point.rx() /= 2;

    p.begin( this );
    p.setRenderHint( QPainter::Antialiasing );
    p.setRenderHint( QPainter::SmoothPixmapTransform );
    p.setTransform( transform );
    p.drawImage( point, composition );
}


void
PrettyCoverWidget::mouseReleaseEvent( QMouseEvent* e )
{
    if (e->button() == Qt::LeftButton)
        emit clicked();
}

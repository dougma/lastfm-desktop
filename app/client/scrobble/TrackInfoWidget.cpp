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

#include "TrackInfoWidget.h"
#include "lib/unicorn/widgets/SpinnerLabel.h"
#include <QSvgRenderer>
#include <QtGui>


TrackInfoWidget::TrackInfoWidget()
{
	ui.spinner = new SpinnerLabel( this );
	ui.spinner->hide();

    clear();    
}


void
TrackInfoWidget::clear()
{
	m_track = Track();
	ui.spinner->hide();
	qDeleteAll( findChildren<AlbumImageFetcher*>() );   
#if 0
    QSvgRenderer svg( QString(":/MainWindow/as.svg") );
    QSize s = svg.defaultSize() * 5;
    m_cover = QImage( s * 5, QImage::Format_ARGB32_Premultiplied );
    QPainter p( &m_cover );
    p.setOpacity( qreal(40)/255 );

    svg.render( &p );
    p.end();
    m_cover = addReflection( m_cover );
#else 
    m_cover = QImage();
#endif
    update();
}


void
TrackInfoWidget::setTrack( const Track& t )
{
	//TODO for scrobbled tracks we should get the artwork out of the track
	if (m_track.album() != t.album())
	{
		m_cover = QImage();
		update();
		
		qDeleteAll( findChildren<AlbumImageFetcher*>() );
		QObject* o = new AlbumImageFetcher( t.album(), Album::ExtraLarge );
		connect( o, SIGNAL(finished( QByteArray )), SLOT(onAlbumImageDownloaded( QByteArray )) );
		o->setParent( this );
		
		ui.spinner->show();
	}

	m_track = t;
}


void
TrackInfoWidget::onAlbumImageDownloaded( const QByteArray& data )
{
    m_cover.loadFromData( data );
    qDebug() << "Image dimensions:" << m_cover.size();

	m_cover = addReflection( m_cover );
	update();

	sender()->deleteLater();
	
	ui.spinner->hide();
}


void
TrackInfoWidget::paintEvent( QPaintEvent* e )
{
    QPainter p( this );
    p.setClipRect( e->rect() );
    p.setRenderHint( QPainter::Antialiasing );
    p.setRenderHint( QPainter::SmoothPixmapTransform );
    
    if (m_cover.isNull())
    {
        if (!m_track.isNull())
            return;
        
        QSvgRenderer svg( QString(":/MainWindow/as.svg") );
        
        QSize s = svg.defaultSize() * 5;
        s.scale( 120, 0, Qt::KeepAspectRatioByExpanding );
        QRect r = QRect( rect().center() - QRect( QPoint(), s ).center(), s );

        p.setOpacity( qreal(40)/255 );
        svg.render( &p, r );
    }
    else {
        
        QTransform trans;
        qreal const scale = qreal(height()) / m_cover.height();
        trans.scale( scale, scale );
        p.setTransform( trans );

        QPointF f = trans.inverted().map( QPointF( width(), 0 ) );

        f.rx() -= m_cover.width();
        f.rx() /= 2;

        p.drawImage( f, m_cover );
    }
}


QImage //static
TrackInfoWidget::addReflection( const QImage &in )
{
    const uint H = 5 * in.height() / 7;
	
    QImage out( in.width(), in.height() + H, QImage::Format_ARGB32_Premultiplied );
    QPainter p( &out );
	QImage in2 = in;
	in2.convertToFormat( QImage::Format_ARGB32_Premultiplied );
    p.drawImage( 0, 0, in2 );
    
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

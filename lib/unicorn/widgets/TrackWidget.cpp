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

#include "TrackWidget.h"
#include "lib/unicorn/TrackImageFetcher.h"
#include <lastfm/Track>
#include <QHBoxLayout>
#include <QLabel>


TrackWidget::TrackWidget()
{    
    QHBoxLayout *h = new QHBoxLayout( this );
    h->addWidget( ui.cover = new QLabel );
    h->addWidget( ui.track = new QLabel );
    h->setMargin( 0 );
    
    ui.cover->setScaledContents( true );
    ui.track->setTextFormat( Qt::RichText );    
}


void
TrackWidget::setTrack( const Track& track )
{
    TrackImageFetcher* fetcher = new TrackImageFetcher( track );
    connect( fetcher, SIGNAL(finished( QImage )), SLOT(onCoverDownloaded( QImage )) );
    fetcher->start();
    
    QString title = track.title();
    QString artist = track.artist();
    QString album = track.album();
    if (title.isEmpty()) title = '[' + tr("Unknown Track") + ']';
    if (track.duration()) title += " </b>(" + track.durationString() + ')';
    if (album.size()) album = tr("from %1").arg( "<span style='color:#fff'>" + album );
    if (artist.isEmpty()) artist = '[' + tr("Unknown Artist") + ']';
    artist = tr("by %1").arg( "<span style='color:#fff'>" + artist + "</span>" );
    
    ui.track->setText( QString("<div style='color:#fff;margin-bottom:2px'><b>") + 
                #ifdef Q_WS_MAC
                      "<span style='font-size:15pt'>" + 
                #endif
                      title + "</b></div>"
                #ifdef Q_WS_MAC
                      "<span style='font-size:10pt'>" + 
                #endif
                      "<span style='color:#878787'><div style='margin-bottom:3px'>" + artist + "</div><div>" + album );
    
    int const h = ui.track->sizeHint().height();
    ui.cover->setFixedSize( h, h );
}


void
TrackWidget::onCoverDownloaded( const QImage& image )
{
    ui.cover->setPixmap( QPixmap::fromImage( image ) );
    sender()->deleteLater();
}

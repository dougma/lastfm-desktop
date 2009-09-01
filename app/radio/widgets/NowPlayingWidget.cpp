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

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QImage>
#include <QPixmap>

#include <lastfm/Track>
#include <lastfm/RadioStation>

#include "lib/unicorn/TrackImageFetcher.h"

#include "NowPlayingWidget.h"
#include "RadioProgressBar.h"

NowPlayingWidget::NowPlayingWidget()
{
    new QVBoxLayout( this );

  //  m_trackWidget = new TrackWidget();
  //  layout()->addWidget(m_trackWidget);
    
    ui.cover = new QLabel();
    ui.cover->setAlignment( Qt::AlignCenter );
    layout()->addWidget( ui.cover );
    
    ((QBoxLayout*)layout())->addStretch();
    
    ui.track = new QLabel();
    layout()->addWidget( ui.track );
    
    ui.album = new QLabel();
    layout()->addWidget( ui.album );
    
    ui.bar = new RadioProgressBar();
    layout()->addWidget(ui.bar);
    connect( this, SIGNAL( tick( qint64 )), ui.bar, SLOT( onRadioTick( qint64 )));
}

void
NowPlayingWidget::onTuningIn( const RadioStation& )
{
}

void
NowPlayingWidget::onTrackSpooled( const Track& t )
{
    ui.bar->onTrackSpooled(t, 0);
}

void
NowPlayingWidget::onTrackStarted( const Track& t )
{
    ui.cover->clear();
    TrackImageFetcher* imageFetcher = new TrackImageFetcher( t );
    connect( imageFetcher, SIGNAL( finished( QImage )), SLOT( onImageFinished( QImage )));
    imageFetcher->start();
    
    ui.track->setText( t.artist() + " - " + t.title() );
    QString albumTitle = t.album();
    if (albumTitle.length()) {
        ui.album->setText( "from " + albumTitle );
        ui.album->show();
    } else {
        ui.album->hide();
    }
}

void 
NowPlayingWidget::onImageFinished( const QImage& image )
{
    ui.cover->setPixmap( QPixmap::fromImage( image ) );
    sender()->deleteLater();
}

void
NowPlayingWidget::onBuffering( int )
{
}

void
NowPlayingWidget::onStopped()
{
}

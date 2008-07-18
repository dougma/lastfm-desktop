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
#include "PlayerEvent.h"
#include "ScrobbleProgressBar.h"


NowPlayingView::NowPlayingView(QWidget *parent)
              : QWidget(parent)
{
    ui.setupUi( this );
    connect( qApp, SIGNAL(event( int, QVariant )), SLOT(onAppEvent( int, QVariant )) );

    ui.vboxLayout->addWidget( new ScrobbleProgressBar );
    ui.vboxLayout->setAlignment( Qt::AlignTop );
}


void
NowPlayingView::setCurrentTrack( const ObservedTrack& track )
{
    ui.artist->setText( track.artist() );
    ui.title->setText( track.title() );
    ui.artwork->setPixmap( track.album().image() );
}


void
NowPlayingView::onAppEvent( int e, const QVariant& v )
{
    switch (e)
    {
    case PlayerEvent::PlaybackEnded:
        // will use empty Track()
    case PlayerEvent::PlaybackStarted:
    case PlayerEvent::TrackChanged:
        setCurrentTrack( v.value<ObservedTrack>() );
        break;
    }
}

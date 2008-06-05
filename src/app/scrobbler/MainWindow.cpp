/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd                                       *
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

#include "MainWindow.h"
#include "PlaybackEvent.h"
#include "PlayerManager.h"
#include "lib/unicorn/Logger.h"
#include "lib/moose/TrackInfo.h"
#include <QTime>
#include <QVariant>


MainWindow::MainWindow()
{
    ui.setupUi( this );

    connect( qApp, SIGNAL(event( int, QVariant )), SLOT(onAppEvent( int, QVariant )) );
    connect( &The::playerManager(), SIGNAL(tick( int )), SLOT(onPlaybackTick( int )) );
}


void
MainWindow::onAppEvent( int e, const QVariant& v )
{
    switch (e)
    {
        case PlaybackEvent::PlaybackStarted:
        case PlaybackEvent::TrackChanged:
            ui.track->setText( v.value<TrackInfo>().toString() );
            break;

        case PlaybackEvent::PlaybackEnded:
            qDebug() << "ENDED";
            ui.track->clear();
            ui.source->clear();
            break;
    }
}


void
MainWindow::onPlaybackTick( int s )
{
    QTime t( 0, 0 );
    t = t.addSecs( s );
    ui.source->setText( t.toString( "mm:ss" ) );
}

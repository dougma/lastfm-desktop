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
#include <lastfm/Track>
#include "lib/unicorn/widgets/TrackWidget.h"
#include "NowPlayingWidget.h"
#include "RadioProgressBar.h"

NowPlayingWidget::NowPlayingWidget()
{
    QVBoxLayout* layout = new QVBoxLayout();

    m_trackWidget = new TrackWidget();
    layout->addWidget(m_trackWidget);

    m_bar = new RadioProgressBar();
    layout->addWidget(m_bar);

    setLayout(layout);
}

void
NowPlayingWidget::onTuningIn( const RadioStation& )
{
}

void
NowPlayingWidget::onTrackSpooled( const Track& t )
{
    m_bar->onTrackSpooled(t, 0);
}

void
NowPlayingWidget::onTrackStarted( const Track& t )
{
    m_trackWidget->setTrack(t);
}

void
NowPlayingWidget::onBuffering( int )
{
}

void
NowPlayingWidget::onStopped()
{
}

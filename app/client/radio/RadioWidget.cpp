/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *    This program is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include "RadioWidget.h"
#include "AudioController.h"
#include <QAction>
#include <QLineEdit>
#include <QToolBar>
#include <QVBoxLayout>


RadioWidget::RadioWidget( QWidget* parent )
           : QWidget( parent )
{
    m_audio = new AudioPlaybackEngine;
    connect( m_audio, SIGNAL(thirtySecondsFromPlaylistEnd()), SLOT(queueMoreTracks()) );

    QToolBar* bar = new QToolBar( this );

    QLineEdit* tuning_dial = new QLineEdit;
    QAction* skip = bar->addAction( "Skip" );
    QAction* stop = bar->addAction( "Stop" );

    QVBoxLayout* v = new QVBoxLayout( this );
    v->addWidget( bar );
    v->addWidget( tuning_dial );

    connect( tuning_dial, SIGNAL(returnPressed()), SLOT(onTunerReturnPressed()) );
    connect( skip, SIGNAL(triggered()), m_audio, SLOT(skip()) );
    connect( stop, SIGNAL(triggered()), m_audio, SLOT(stop()) );
}


void
RadioWidget::onTunerReturnPressed()
{
    QString url = static_cast<QLineEdit*>(sender())->text();
    play( RadioStation( url ) );
}


void
RadioWidget::play( const RadioStation& station )
{
    m_tuner = Tuner( station );
    queueMoreTracks();
    m_audio->play();
}


void
RadioWidget::queueMoreTracks()
{
    QList<Track> tracks = m_tuner.fetchNextPlaylist();
    m_audio->queue( tracks );
}

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

#include "lib/unicorn/Track.h"
#include <Phonon>
#include <QList>
#include <QMap>
#include <QObject>
#include <QUrl>


class AudioPlaybackEngine : public QObject
{
    Q_OBJECT
    
public:
    AudioPlaybackEngine();
    
public slots:
    void queue( const QList<Track>& );
    void clearQueue();

    void play();
    void skip();
    void stop();

signals:
    void trackStarted( const Track& );
    /** queue more tracks or... */
    void queueStarved();
    /** ...playback ends */
    void playbackEnded();

private slots:
    void onTrackStarted( const Phonon::MediaSource& );
    void onPhononStateChanged( Phonon::State, Phonon::State );

private:
    Phonon::MediaObject *m_mediaObject;
    Phonon::AudioOutput *m_audioOutput;

    QMap<QUrl, Track> m_queue;
};

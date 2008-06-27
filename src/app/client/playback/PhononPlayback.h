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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02111-1307, USA.          *
 ***************************************************************************/
 
#ifndef PHONON_PLAYBACK_H
#define PHONON_PLAYBACK_H

#include <QObject>
#include <QBuffer>
#include <QUrl>
#include <phonon/phononnamespace.h>

namespace Phonon
{
    class MediaObject;
    class AudioOutput;
    class MediaSource;
}


/** @author Adam Renberg <adam@last.fm>
  * @author Max Howell <max@last.fm>
  */

class PhononPlayback : public QObject
{
    Q_OBJECT

signals:
    void event( int, const QVariant& );

public:
    PhononPlayback();

public slots:
    // adds track to the queue being played. 
    void enqueueTrack( const class QUrl& );

    void startPlayback();
    void pausePlayback();
    void stopPlayback();
    void skip();

public:
    // resets internals to a state similar to a newly created instance
    void reset();

protected slots:
    void privateOnStateChanged( Phonon::State newState, Phonon::State oldState );
    void privateOnCurrentSourceChanged( const Phonon::MediaSource& source );

private:
    Phonon::MediaObject *m_mediaObject;
    Phonon::AudioOutput *m_audioOutput;
};

#endif


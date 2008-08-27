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

#ifndef RADIOCONTROLLER_H
#define RADIOCONTROLLER_H

#include <QList>
#include <QObject>
#include "lib/types/Track.h"
#include "lib/radio/RadioStation.h"
#include "lib/DllExportMacro.h"
#include "phononnamespace.h"
namespace Phonon
{
	class MediaObject;
	class AudioOutput;
	class MediaSource;
}


class RADIO_DLLEXPORT Radio : public QObject
{
    Q_OBJECT

public:
    Radio( Phonon::AudioOutput* );

	Phonon::AudioOutput* audioOutput() const { return m_audioOutput; }
	Phonon::State state() const;
	
public slots:
    void play( const RadioStation& );
    void stop();
    void skip();
	void pause();
	void unpause();

signals:
	void tuningIn( const QString& title );
	void tuned( const QString& title );
	void preparing( const Track& );
	/** buffer fill status as a percentage, you can get this mid-track too, if 
	  * so, see playbackResumed() */
	void buffering( int );
    void trackStarted( const Track& );
	/** stop() was called or a serious radio error occurred */
    void playbackEnded();
	
	void playbackPaused();
	
    /** follows pause and buffering */
    void playbackResumed();

	/** playback may still be occuring, so you may want to hold the error
	  * eg. NotEnoughContent mid-track means we can't get anymore tracks after
	  * the current playlist finishes */
	void error( Ws::Error );

private slots:
    void enqueue( const QList<Track>& );
    void onPhononStateChanged( Phonon::State, Phonon::State );
	void onBuffering( int );
	
private:
	class Tuner* m_tuner;
	Phonon::AudioOutput* m_audioOutput;
	Phonon::MediaObject* m_mediaObject;
	
    QMap<QUrl, Track> m_queue;
};

#endif //RADIOCONTROLLER_H

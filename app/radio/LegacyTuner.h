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
#ifndef LEGACY_TUNER_H
#define LEGACY_TUNER_H

#include <lastfm/Track>
#include <lastfm/RadioStation>
#include <lastfm/ws.h>
using lastfm::RadioStation;

/** certain stuff can only be played with the legacy tuner */


class LegacyTuner : public QObject
{
	Q_OBJECT
	
public:
	/** You need to have assigned Ws::* for this to work, creating the tuner
	  * automatically fetches the first 5 tracks for the station */
    LegacyTuner( const RadioStation&, const QString& password_md5 );

    lastfm::Track takeNextTrack();

signals:
    void title( const QString& );
    void trackAvailable();
    void error( lastfm::ws::Error );

private slots:
	void onHandshakeReturn();
    void onAdjustReturn();
	void onGetPlaylistReturn();

private:
	/** Tries again up to 5 times
	  * @returns true if we tried again, otherwise you should emit error */
	bool tryAgain();
    bool fetchFiveMoreTracks();

    class QNetworkAccessManager* m_nam;
	uint m_retry_counter;
    RadioStation m_station;
    QByteArray m_session;
    QList<lastfm::Track> m_queue;
};

#endif

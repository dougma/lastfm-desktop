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

#ifndef LEGACY_TUNER_H
#define LEGACY_TUNER_H

#include <lastfm/Track>
#include "RadioStation.h"

/** certain stuff can only be played with the legacy tuner */


class LASTFM_RADIO_DLLEXPORT LegacyTuner
{
	Q_OBJECT
	
public:
	/** You need to have assigned Ws::* for this to work, creating the tuner
	  * automatically fetches the first 5 tracks for the station */
    LegacyTuner( const RadioStation&, const QString& password_md5 );

    Track takeNextTrack();

signals:
    void title( const QString& );
    void trackAvailable();
    void error( Ws::Error );

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

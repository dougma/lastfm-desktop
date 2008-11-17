/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd                                       *
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

#ifndef XSPF_PLAYER_H
#define XSPF_PLAYER_H

#include <QList>
#include <QObject>
#include <phonon/phononnamespace.h>
#include <lastfm/types/Track.h>
#include <lastfm/radio/RadioStation.h>

namespace Phonon
{
	class MediaObject;
	class AudioOutput;
	class MediaSource;
}


class XspfPlayer : public QObject
{
    Q_OBJECT;

public:
    XspfPlayer(Phonon::AudioOutput*, class Resolver* resolver = 0);
    ~XspfPlayer();

    void play(QUrl);
    void play(QString xml);

public slots:
//    void play( const RadioStation& );
    void skip();
    void stop();

signals:
    void tuningIn(const RadioStation&);
    void trackSpooled(const Track&); /** and we're now prebuffering */
    void trackStarted(const Track&);
    void buffering(int);
    void stopped();

private slots:
    void onFinished();
    void phononEnqueue();
    void onPhononStateChanged(Phonon::State, Phonon::State);
    void onPhononCurrentSourceChanged(const Phonon::MediaSource&);
    void onResolveComplete(const Track&);

private:
    void handleXspf(const QDomElement& playlistElement);
    void clear();

	Phonon::AudioOutput* m_audioOutput;
	Phonon::MediaObject* m_mediaObject;
	Track m_track;
	RadioStation m_station;
    class Resolver *m_resolver;
    class QNetworkReply *m_reply;

    bool m_bErrorRecover;
    QList<Track> m_queue;
};

#endif
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

#include "XspfPlayer.h"
#include "lib/lastfm/ws/WsAccessManager.h"
#include "lib/lastfm/types/Xspf.h"
#include "lib/lastfm/radio/Resolver.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>

#define XSPF_INITIAL_RESOLVER_WAIT_MS 3000
#define XSPF_INTERTRACK_RESOLVER_WAIT_MS 3000

XspfPlayer::XspfPlayer(Phonon::AudioOutput* output, class Resolver* resolver)
: m_audioOutput(output)
, m_resolver(resolver)
, m_reply(0)
, m_bErrorRecover(false)
{
    m_mediaObject = new Phonon::MediaObject( this );
    m_mediaObject->setTickInterval( 1000 );
    connect( m_mediaObject, SIGNAL(stateChanged( Phonon::State, Phonon::State )), SLOT(onPhononStateChanged( Phonon::State, Phonon::State )) );
	connect( m_mediaObject, SIGNAL(currentSourceChanged( const Phonon::MediaSource &)), SLOT(onPhononCurrentSourceChanged( const Phonon::MediaSource &)) );
    connect( m_mediaObject, SIGNAL(aboutToFinish()), SLOT(phononEnqueue()) ); // this fires when the whole queue is about to finish
    Phonon::createPath( m_mediaObject, m_audioOutput );

    if (m_resolver) {
        connect( m_resolver, SIGNAL(resolveComplete( Track )), SLOT(onResolveComplete( Track )) );
    }
}

XspfPlayer::~XspfPlayer()
{
    clear();
}

void 
XspfPlayer::play(QUrl url)
{
    clear();
    m_station = RadioStation(url);
    emit tuningIn(m_station);

    m_reply = (new WsAccessManager(this))->get(QNetworkRequest(url));
    connect(m_reply, SIGNAL(finished()), SLOT(onFinished()));
}

void
XspfPlayer::play(QString xmlXspf)
{
    clear();
    emit tuningIn(m_station);

    QDomDocument xml(xmlXspf);
    QDomElement doc(xml.documentElement());
    if (doc.tagName() == "playlist") {
        handleXspf(doc);
    }
}


void
XspfPlayer::onFinished()
{
    QDomDocument xmldoc;
    xmldoc.setContent(m_reply);
    QDomElement docElement(xmldoc.documentElement());
    if (docElement.tagName() == "playlist") {
        handleXspf(docElement);
    } else if (docElement.tagName() == "lfm") {
        handleXspf(docElement.firstChildElement("playlist"));
    }
    emit stopped();

    delete m_reply;
    m_reply = 0;
}

void 
XspfPlayer::handleXspf(const QDomElement& playlistElement)
{
    Xspf xspf(playlistElement, Track::Player);
    m_station.setTitle(xspf.title());

    emit tuningIn(m_station);

    m_queue = xspf.tracks();
    if (m_queue.size()) {
        if (m_resolver) {
            foreach(const Track& t, m_queue) {
                m_resolver->resolve(t);
            }
        }
        phononEnqueue();
    } else {
        // hmm... nothing to play
        emit stopped();
    }
}

void
XspfPlayer::onResolveComplete(const Track& t)
{
    Q_ASSERT(!m_queue.empty());
    if (t == m_queue[0]) {
        phononEnqueue();
    }
}

// maybe add a track to phonon 
// (if the phonon queue is empty, and the next track in the queue has resolved)
void
XspfPlayer::phononEnqueue()
{
    if (!m_queue.isEmpty() && m_mediaObject->queue().isEmpty()) {
        Track t = m_queue.first();
        if (m_resolver && m_resolver->stillResolving(t)) {
            return;
        }

        // it would be really nice if we could throw any url at phonon, but 
        // an invalid url doesn't produce the proper state changes, ie: we
        // don't get to the error state. (noticed in qt 4.4.3) hence:
        if (!t.url().isValid()) {
            qDebug() << t << "invalid url:" << t.url();
            MutableTrack(t).setUrl(QUrl("urn:rubbish"));
            Q_ASSERT(t.url().isValid());
        } else {
            QString sFile(t.url().toLocalFile().replace('/','\\'));
            qDebug() << t << t.url();
        }

        Phonon::MediaSource ms(t.url());
        // it seems important to make this distinction:
        if (m_mediaObject->state() == Phonon::PlayingState) {
            m_mediaObject->enqueue(ms);
        } else {
            m_mediaObject->setCurrentSource(ms);
        }

        m_mediaObject->play();
    }
}


// onPhononCurrentSourceChanged happens always (even if the source ends up 
// being unplayable), so we use it to update our now playing track.
void
XspfPlayer::onPhononCurrentSourceChanged(const Phonon::MediaSource&)
{
    m_track = m_queue.takeFirst();
    MutableTrack( m_track ).stamp();
}

void
XspfPlayer::onPhononStateChanged(Phonon::State newstate, Phonon::State /*oldstate*/)
{
    switch (newstate)
    {
        case Phonon::ErrorState:
			if (m_mediaObject->errorType() == Phonon::FatalError)
				qCritical() << m_mediaObject->errorString();

            m_bErrorRecover = true;
            m_mediaObject->stop();  // seems we need to clear the error state before trying to play again.
            break;
			
		case Phonon::PausedState:
            if (m_queue.empty()) {
                emit stopped();
            }
            break;
			
        case Phonon::StoppedState:
            if (m_bErrorRecover) {
                m_bErrorRecover = false;
                skip();
            }
            break;
			
        case Phonon::BufferingState:
            emit trackSpooled( m_track );
            break;

		case Phonon::PlayingState:
            emit trackStarted( m_track );
            break;

		case Phonon::LoadingState:
			break;
    }
}

void 
XspfPlayer::skip()
{
	QList<Phonon::MediaSource> q = m_mediaObject->queue();
    if (q.size()) {
		Phonon::MediaSource source = q.takeFirst();
		m_mediaObject->setCurrentSource( source );
		m_mediaObject->setQueue( q );
        m_mediaObject->play();
    } else {
        phononEnqueue();
    }
}

void 
XspfPlayer::stop()
{
    // blockSignals to prevent the error state due to setting current source to null
    m_mediaObject->blockSignals( true ); 
	m_mediaObject->stop();
	m_mediaObject->clearQueue();
	m_mediaObject->setCurrentSource( QUrl() );
    m_mediaObject->blockSignals( false );
}

void 
XspfPlayer::clear()
{
    if (m_reply) {
        delete m_reply;
        m_reply = 0;
    }

    stop();

    if (m_resolver) {
        foreach(const Track& t, m_queue) {
            m_resolver->stopResolving(t);
        }
    }
    m_queue.clear();
    m_track = Track();
    m_station = RadioStation();
}


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

#include "RadioPlayer.h"
//#include "lib/radio/Radio.h"
#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#include <phonon/mediasource.h>
#include <phonon/backendcapabilities.h>
#include <QDebug>
#include <QDir>

QString password, username;


RadioPlayer::RadioPlayer( const QString& _username, const QString& _password )
{
    username = _username; password = _password;

    m_audioOutput = new Phonon::AudioOutput( Phonon::MusicCategory, this );
    m_mediaObject = new Phonon::MediaObject( this );
    
    m_mediaObject->setTickInterval( 1000 );

    Phonon::createPath( m_mediaObject, m_audioOutput );
}


void
RadioPlayer::play( const QString& url )
{
    qRegisterMetaType<QList<Radio::Track> >( "QList<Radio::Track>" );

    m_radio = new Radio( username, password );
    connect( m_radio, SIGNAL(tracks( QList<Radio::Track> )), SLOT(onTracksReady( QList<Radio::Track> )) );
    m_radio->tuneIn( url );
}


void
RadioPlayer::skip()
{
    m_mediaObject->setCurrentSource( m_mediaObject->queue().front() );
    m_mediaObject->play();
}


void
RadioPlayer::stop()
{
    m_mediaObject->stop();
}


void
RadioPlayer::onTracksReady( const QList<Radio::Track>& tracks )
{
    QList<QUrl> urls;
    foreach (const Radio::Track& t, tracks)
        urls += t.location;

    m_mediaObject->enqueue( urls );
    m_mediaObject->play();
}


void
RadioPlayer::onAboutToFinishPlaylist()
{
    m_radio->fetchNextPlaylist();
}

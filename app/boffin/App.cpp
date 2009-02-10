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
 
#include "App.h"

#include "TrackTagUpdater.h"
#include "LocalRqlPlugin.h"
#include "TrackResolver.h"
#include "Resolver.h"
#include "lib/lastfm/radio/Radio.h"

#include <phonon/audiooutput.h>
#include <phonon/backendcapabilities.h>

#include "XspfResolvingTrackSource.h"


App::App( int& argc, char* argv[] )
    :Unicorn::Application( argc, argv )
{
    m_trackTagUpdater = TrackTagUpdater::create(
        "http://musiclookup.last.fm/trackresolve",
        100,        // number of days track tags are good 
        5);         // 5 minute delay between web requests


    m_localRql = new LocalRqlPlugin();
    m_localRql->init();

    m_trackResolver = new TrackResolver();
    m_resolver = new Resolver( QList<ITrackResolverPlugin*>() << m_trackResolver );

    //////////////////////////////////////

    Phonon::AudioOutput* audioOutput = new Phonon::AudioOutput( Phonon::MusicCategory, this );
	audioOutput->setVolume( 1.0 /* Settings().volume() */ );

    QString audioOutputDeviceName; /* = moose::Settings().audioOutputDeviceName(); */
    foreach (Phonon::AudioOutputDevice d, Phonon::BackendCapabilities::availableAudioOutputDevices()) {
        audioOutput->setOutputDevice( d );
        break;
    }

	m_radio = new Radio( audioOutput );
//  connect( m_radio, SIGNAL(tuningIn( RadioStation )), m_stateMachine, SLOT(onRadioTuningIn( RadioStation )) );
//  connect( m_radio, SIGNAL(trackSpooled( Track )), m_stateMachine, SLOT(onRadioTrackSpooled( Track )) );
//  connect( m_radio, SIGNAL(trackStarted( Track )), m_stateMachine, SLOT(onRadioTrackStarted( Track )) );
//  connect( m_radio, SIGNAL(stopped()), m_stateMachine, SLOT(onRadioStopped()) );
//  connect( m_radio, SIGNAL(error( int, QVariant )), SLOT(onRadioError( int, QVariant )) );

    ////

    if (argc > 1) {
        openXspf( argv[1] );
    }

    
}

void
App::openXspf( QString filename )
{
    XspfResolvingTrackSource* src = new XspfResolvingTrackSource( m_resolver, filename );
    m_radio->play( RadioStation( "XSPF" ), src );
    src->start();
}
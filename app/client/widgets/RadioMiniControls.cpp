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

#include "RadioMiniControls.h"
#include "PlayerEvent.h"
#include "ObservedTrack.h"
#include <phonon/volumeslider.h>


RadioMiniControls::RadioMiniControls()
{
    ui.setupUi(this);
	
	ui.play->setCheckedIcon( QIcon( ":/stop.png" ));
	
    layout()->addWidget( ui.volume = new Phonon::VolumeSlider );
	
	connect( qApp, SIGNAL(event( int, const QVariant&)), SLOT( onAppEvent( int, const QVariant&)) );
	connect( ui.play, SIGNAL( clicked()), SLOT( onPlayClicked()) );
	
	ui.volume->setMinimumWidth( ui.play->width() + ui.skip->width() );
}


void 
RadioMiniControls::onAppEvent( int e, const QVariant& d )
{
	switch ( e ) 
	{
		case PlayerEvent::PlaybackSessionStarted:
			if( d.toString() == "ass" )
				ui.play->setChecked( true );
			break;
		
		case PlayerEvent::PlaybackSessionEnded:
		case PlayerEvent::PlaybackPaused:
			break;

		default:
			break;
	}
}


void
RadioMiniControls::onPlayClicked()
{
	if( !ui.play->isChecked() )
		emit stop();
}
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "RadioMiniControls.h"
#include "widgets/ImageButton.h"
#include <QApplication>
#include <QHBoxLayout>
#include <phonon/volumeslider.h>
#include "the/radio.h"


RadioMiniControls::RadioMiniControls()
{
    QHBoxLayout* h = new QHBoxLayout( this );
    h->addWidget( ui.play = new ImageButton( ":/play.png" ) );
    h->addWidget( ui.skip = new ImageButton( ":/skip.png" ) );
    h->setSpacing( 0 );

    ui.play->setPixmap( ":/stop.png", QIcon::On );
    ui.play->setCheckable( true );
    ui.play->setChecked( false );
    ui.volume = new Phonon::VolumeSlider;
	ui.volume->setMinimumWidth( ui.play->sizeHint().width() + ui.skip->sizeHint().width() );
	
    h->addWidget( ui.volume );
	
	connect( &The::radio(), SIGNAL(stopped()), SLOT(onRadioStopped()) );
    connect( &The::radio(), SIGNAL(tuningIn( const RadioStation&)), SLOT( onRadioTuningIn( const RadioStation&)));
	connect( ui.play, SIGNAL( clicked()), SLOT( onPlayClicked()) );
}


void
RadioMiniControls::onRadioStopped()
{
    ui.play->setChecked( false );
}


void 
RadioMiniControls::onRadioTuningIn( const RadioStation& )
{
    ui.play->setChecked( true );
}


void
RadioMiniControls::onPlayClicked()
{
	if (!ui.play->isChecked())
		emit stop();
    else
        emit play();
}

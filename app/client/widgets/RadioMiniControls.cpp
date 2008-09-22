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
#include "PlayerState.h"
#include  "widgets/ImageButton.h"
#include <QApplication>
#include <QHBoxLayout>
#include <phonon/volumeslider.h>


RadioMiniControls::RadioMiniControls()
{
    QHBoxLayout* h = new QHBoxLayout( this );
    h->addWidget( ui.play = new ImageButton( ":/play.png" ) );
    h->addWidget( ui.skip = new ImageButton( ":/skip.png" ) );
    h->setSpacing( 0 );

    ui.play->setPixmap( ":/stop.png", QIcon::On );
    ui.volume = new Phonon::VolumeSlider;
	ui.volume->setMinimumWidth( ui.play->sizeHint().width() + ui.skip->sizeHint().width() );
	
    layout()->addWidget( ui.volume );
	
	connect( qApp, SIGNAL(stateChanged( State )), SLOT(onStateChanged( State )) );
	connect( ui.play, SIGNAL( clicked()), SLOT( onPlayClicked()) );
    
    onStateChanged( Stopped );
}


void
RadioMiniControls::onStateChanged( State state )
{
	switch (state) 
	{
		case Playing:
            break;

        case TuningIn:
            show();
            ui.play->setChecked( true );
			break;
		
		case Stopped:
            hide();
            ui.play->setChecked( false );
			break;
            
        default:
            break;
	}
}


void
RadioMiniControls::onPlayClicked()
{
	if (!ui.play->isChecked())
		emit stop();
}

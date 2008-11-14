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

#include "RadioControls.h"
#include "widgets/ImageButton.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "widgets/UnicornVolumeSlider.h"
#include "UnicornWidget.h"
#include "the/radio.h"

#include <QSlider>
#include <QToolButton>
#include <QPainter>
#include <QPaintEvent>

RadioControls::RadioControls()
{
    QVBoxLayout* v = new QVBoxLayout( this );
    v->setContentsMargins( 0, 0, 0, 0 );
    v->setSpacing( 0 );
    QWidget* hLayoutWidget = new QWidget( this );
    QHBoxLayout* h = new QHBoxLayout( hLayoutWidget );
    h->setContentsMargins( 0, 0, 0, 0 );
    h->setSpacing( 0 );
    
    h->addWidget( ui.play = new ImageButton( ":/RadioControls/play/rest.png" ) );
    ui.play->setPixmap( ":/RadioControls/play/onpress.png", QIcon::Active );
    
    ui.play->setPixmap( ":/RadioControls/stop/rest.png", QIcon::On );
    ui.play->setPixmap( ":/RadioControls/stop/onpress.png", QIcon::On, QIcon::Active );
    ui.play->setCheckable( true );
    ui.play->setChecked( false );
    
    h->addWidget( ui.skip = new ImageButton( ":/RadioControls/skip/rest.png" ) );

    ui.skip->setPixmap( ":/RadioControls/skip/onpress.png", QIcon::Active );
    ui.play->setCheckable( true );
    ui.play->setChecked( false );

    h->insertStretch( 0, 1 );
    h->insertStretch( -1, 1 );
    
    v->addWidget( hLayoutWidget );
    v->insertStretch( 0, 1 );
    v->insertStretch( -1, 1 );
	
	connect( &The::radio(), SIGNAL(stopped()), SLOT(onRadioStopped()) );
    connect( &The::radio(), SIGNAL(tuningIn( const RadioStation&)), SLOT( onRadioTuningIn( const RadioStation&)));
	connect( ui.play, SIGNAL( clicked()), SLOT( onPlayClicked()) );
    connect( ui.skip, SIGNAL( clicked()), SIGNAL(skip()));
    setAutoFillBackground( true );
    UnicornWidget::paintItBlack( this );
}


void
RadioControls::onRadioStopped()
{
    ui.play->setChecked( false );
}


void 
RadioControls::onRadioTuningIn( const RadioStation& )
{
    ui.play->setChecked( true );
}


void
RadioControls::onPlayClicked()
{
	if (!ui.play->isChecked())
		emit stop();
    else
        emit play();
}
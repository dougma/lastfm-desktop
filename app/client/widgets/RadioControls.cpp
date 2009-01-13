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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "RadioControls.h"
#include "widgets/ImageButton.h"
#include <QApplication>
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
    QHBoxLayout* h = new QHBoxLayout( this );
    h->setContentsMargins( 12, 0, 12, 0 );
    h->setSpacing( 5 );
    
    h->addWidget( ui.play = new ImageButton( ":/RadioControls/play/rest.png" ) );
    ui.play->setPixmap( QPixmap(":/RadioControls/play/onpress.png"), QIcon::Off, QIcon::Active );
    
    ui.play->setPixmap( QPixmap(":/RadioControls/stop/rest.png"), QIcon::On );
    ui.play->setPixmap( QPixmap(":/RadioControls/stop/onpress.png"), QIcon::On, QIcon::Active );
    ui.play->setCheckable( true );
    ui.play->setChecked( false );
    
    h->addWidget( ui.skip = new ImageButton( ":/RadioControls/skip/rest.png" ) );

    ui.skip->setPixmap( QPixmap(":/RadioControls/skip/onpress.png"), QIcon::Off, QIcon::Active );
    ui.play->setCheckable( true );
    ui.play->setChecked( false );

	connect( &The::radio(), SIGNAL(stopped()), SLOT(onRadioStopped()) );
    connect( &The::radio(), SIGNAL(tuningIn( const RadioStation&)), SLOT( onRadioTuningIn( const RadioStation&)));
	connect( ui.play, SIGNAL( clicked()), SLOT( onPlayClicked()) );
    connect( ui.skip, SIGNAL( clicked()), SIGNAL(skip()));

    setFixedWidth( sizeHint().width() );

    setAutoFillBackground( false );
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


void 
RadioControls::resizeEvent( QResizeEvent* event )
{
    if( event->oldSize().height() == event->size().height() )
        return;
    
    QPalette p = palette();
    QLinearGradient lg( 0, 0, 0, event->size().height() / 2);
    lg.setColorAt( 0, 0x404040 );
    lg.setColorAt( 1, 0x222222 );
    p.setBrush( QPalette::Window, lg );
    
    QLinearGradient lg2( 0, event->size().height() / 2, 0, event->size().height());
    lg2.setColorAt( 0, 0x0e0e0e );
    lg2.setColorAt( 1, 0x1b1a1a );
    p.setBrush( QPalette::AlternateBase, lg2 );
    
    setPalette( p );
}


void 
RadioControls::paintEvent( QPaintEvent* event )
{
    QPainter p( this );
    p.setClipRect( event->rect() );
    p.setRenderHint( QPainter::Antialiasing );
    
    p.fillRect( rect().adjusted( 6, 0, 0, rect().height() / -2), palette().brush( QPalette::Window));
    p.fillRect( rect().adjusted( 0, (rect().height() / 2) - 6, 6 - rect().width(), rect().height() / -2), palette().brush( QPalette::Window ));
    p.fillRect( rect().adjusted( 0, (rect().height() / 2) + 6, 6 - rect().width(), rect().height() / -2), palette().brush( QPalette::AlternateBase ));
    p.fillRect( rect().adjusted( 6, rect().height() / 2, 0, 0), palette().brush( QPalette::AlternateBase));
    
    p.setPen( Qt::transparent );
    p.setBrush( palette().brush( QPalette::Window ) );
    p.drawRoundedRect( rect().adjusted( 0, 0, 12 - rect().width(), rect().height() / -2 ), 4, 4 );
    
    p.setBrush( palette().brush( QPalette::AlternateBase ));
    p.drawRoundedRect( rect().adjusted( 0, rect().height() / 2, 12 - rect().width(), 0 ), 4, 4 );
}

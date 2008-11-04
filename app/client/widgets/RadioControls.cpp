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
#include <QHBoxLayout>
#include <phonon/volumeslider.h>
#include "UnicornWidget.h"
#include "the/radio.h"

#include <QSlider>
#include <QToolButton>
#include <QPainter>
#include <QPaintEvent>

RadioControls::RadioControls()
              : m_volumeSlider( 0 )
{
    QHBoxLayout* h = new QHBoxLayout( this );
    
    h->addWidget( ui.play = new ImageButton( ":/RadioControls/play/rest.png" ) );
    ui.play->setPixmap( ":/RadioControls/play/onpress.png", QIcon::Active );
    
    ui.play->setPixmap( ":/RadioControls/stop/rest.png", QIcon::On );
    ui.play->setPixmap( ":/RadioControls/stop/onpress.png", QIcon::On, QIcon::Active );
    ui.play->setCheckable( true );
    ui.play->setChecked( false );
    
    h->addWidget( ui.skip = new ImageButton( ":/RadioControls/skip/rest.png" ) );
    ui.skip->setPixmap( ":/RadioControls/skip/onpress.png", QIcon::Active );
    
    h->setSpacing( 1 );

    ui.play->setCheckable( true );
    ui.play->setChecked( false );
    
    ui.volume = new Phonon::VolumeSlider;
	ui.volume->setMinimumWidth( ui.play->sizeHint().width() + ui.skip->sizeHint().width() );
    ui.volume->setMuteVisible( true );
    ui.volume->setAudioOutput( The::radio().audioOutput());
    
    QToolButton* muteButton = ui.volume->findChild<QToolButton*>();
    if( muteButton )
        muteButton->installEventFilter( this );
    
    m_volumeSlider = ui.volume->findChild<QSlider*>();
    if( m_volumeSlider )
    {
        m_volumeSlider->installEventFilter( this );
        connect( m_volumeSlider, SIGNAL(valueChanged(int)), SLOT(onVolumeValueChanged(int)));
    }
	
    h->addWidget( ui.volume );
	
	connect( &The::radio(), SIGNAL(stopped()), SLOT(onRadioStopped()) );
    connect( &The::radio(), SIGNAL(tuningIn( const RadioStation&)), SLOT( onRadioTuningIn( const RadioStation&)));
	connect( ui.play, SIGNAL( clicked()), SLOT( onPlayClicked()) );
    connect( ui.skip, SIGNAL( clicked()), SIGNAL(skip()));
    setAutoFillBackground( true );
    UnicornWidget::paintItBlack( this );
}


void
RadioControls::onVolumeValueChanged( int v )
{
    static_cast<QWidget*>(sender()->parent())->update();
}


bool 
RadioControls::eventFilter( QObject* watched, QEvent* event )
{
    if( QSlider* slider = qobject_cast<QSlider*>( watched ))
    {
        if( sliderEventFilter( slider, event ))
            return true;
    }
    
    if( QToolButton* button = qobject_cast<QToolButton*>( watched ))
    {
        if( toolButtonEventFilter( button, event ))
            return true;
    }

   return QWidget::eventFilter( watched, event );
}


bool
RadioControls::sliderEventFilter( QSlider* slider, QEvent* e ) const
{
    switch( e->type())
    {
        case QEvent::Paint:
        {
            QPaintEvent* event = static_cast< QPaintEvent*>(e);
            QPainter p( slider );
            p.setClipRect( event->rect());
            
            int midY = qRound( slider->rect().height() / 2.0f );
            p.setPen( QColor( 0x0c, 0x0c, 0x0c ));
            p.drawLine( slider->rect().left(), midY,
                       slider->rect().right(), midY);
            
            p.setPen( QColor( 0x37, 0x37, 0x37 ));
            p.drawLine( slider->rect().left(), midY + 1,
                       slider->rect().right(), midY + 1 );
            
            
            QPixmap pm( ":/RadioControls/volume/handle.png" );
            
            midY++;
            int handleX = slider->rect().x() + ( pm.width() / 2.0f ) + ( (slider->rect().width() - qreal(pm.width())) * (qreal(slider->value()) / qreal(slider->maximum())));
            QRect handleRect( handleX - (qreal(pm.width()) / 2.0f), midY - (qreal(pm.height()) / 2.0f ), pm.width(), pm.height());
            p.drawPixmap( handleRect, pm );
            
            return true;
        }
            
        default: return false;
            
    }
}


bool 
RadioControls::toolButtonEventFilter( QToolButton* b, QEvent* e ) const
{
    switch( e->type())
    {
        case QEvent::Paint:
        {
            QPaintEvent* event = static_cast< QPaintEvent*>(e);
            QPainter p( b );
            p.setClipRect( event->rect());
            
            QPixmap pm( ":/RadioControls/volume/icon.png" );
            
            QRect pmRect = pm.rect().translated( (b->width() / 2) - (pm.width() / 2 ), 
                                                 (b->height() / 2) - (pm.height() / 2 ) + 1);
            
            p.drawPixmap( pmRect, pm );
            return true;
        }
            
        default: return false;
            
    }
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

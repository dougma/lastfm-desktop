/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "UnicornVolumeSlider.h"
#include <QPaintEvent>
#include <QPainter>
#include <QSlider>
#include <QLayout>


void 
UnicornVolumeSlider::installFilters()
{
    QToolButton* muteButton = findChild<QToolButton*>();
    if( muteButton )
        muteButton->installEventFilter( this );
    
    QSlider* volumeSlider = findChild<QSlider*>();
    if( volumeSlider )
    {
        volumeSlider->setOrientation( Qt::Vertical );
        volumeSlider->installEventFilter( this );
        connect( volumeSlider, SIGNAL(valueChanged(int)), SLOT(onVolumeValueChanged(int)));
    }
    
    setFixedWidth( 30 );
    layout()->setContentsMargins( 0, 10, 0, 10);
    setAutoFillBackground( false );
}


bool
UnicornVolumeSlider::eventFilter( QObject* watched, QEvent* event )

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
    
    return Phonon::VolumeSlider::eventFilter( watched, event );
}


bool
UnicornVolumeSlider::sliderEventFilter( QSlider* slider, QEvent* e ) const
{
    switch( e->type())
    {
        case QEvent::Paint:
        {
            QPaintEvent* event = static_cast< QPaintEvent*>(e);
            QPainter p( slider );
            p.setClipRect( event->rect());
            
            int midX = qRound( slider->rect().width() / 2.0f );
            
            QPixmap pm( ":/RadioControls/volume/handle.png" );
            p.setPen( QPen( QColor( 0x10, 0x10, 0x10 ), 2, Qt::SolidLine, Qt::RoundCap ) );
            p.drawLine( midX, slider->rect().top() + (pm.height() / 2),
                       midX, slider->rect().bottom() - (pm.height() / 2) );
            
            
            int handleY = slider->height() - pm.height() - ((slider->height() - pm.height()) * ( slider->value() / qreal(slider->maximum())));
            QRect handleRect( midX - (qreal(pm.width()) / 2.0f), handleY, pm.width(), pm.height());
            p.drawPixmap( handleRect, pm );
            return true;
        }
            
        default: return false;
            
    }
}


bool 
UnicornVolumeSlider::toolButtonEventFilter( QToolButton* b, QEvent* e ) const
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
UnicornVolumeSlider::paintEvent( QPaintEvent* event )
{
    QPainter p( this );
    p.setClipRect( event->rect() );
    p.setRenderHint( QPainter::Antialiasing );
    
    p.fillRect( rect().adjusted( 0, 0, -6, rect().height() / -2), palette().brush( QPalette::Window));
    p.fillRect( rect().adjusted( rect().width() - 6, 6, 0, rect().height() / -2), palette().brush( QPalette::Window));
    p.fillRect( rect().adjusted( rect().width() - 6, rect().height() / 2, 0, -6), palette().brush( QPalette::AlternateBase ));
    p.fillRect( rect().adjusted( 0, rect().height() / 2, -6, 0), palette().brush( QPalette::AlternateBase));
    
    p.setPen( Qt::transparent );
    p.setBrush( palette().brush( QPalette::Window ) );
    p.drawRoundedRect( rect().adjusted( rect().width() - 12, 0, 0, rect().height() / -2), 4, 4 );
    
    p.setBrush( palette().brush( QPalette::AlternateBase ));
    p.drawRoundedRect( rect().adjusted( rect().width() - 12, rect().height() / 2, 0, 0 ), 4, 4 );
}


void 
UnicornVolumeSlider::resizeEvent( QResizeEvent* event )
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
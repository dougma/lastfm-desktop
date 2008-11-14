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

#include "UnicornVolumeSlider.h"
#include <QPaintEvent>
#include <QPainter>
#include <QSlider>


void 
UnicornVolumeSlider::installFilters()
{
    QToolButton* muteButton = findChild<QToolButton*>();
    if( muteButton )
        muteButton->installEventFilter( this );
    
    QSlider* volumeSlider = findChild<QSlider*>();
    if( volumeSlider )
    {
        volumeSlider->installEventFilter( this );
        connect( volumeSlider, SIGNAL(valueChanged(int)), SLOT(onVolumeValueChanged(int)));
    }
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
            
            int midY = qRound( slider->rect().height() / 2.0f );
            p.setPen( QColor( 0x10, 0x10, 0x10 ));
            p.drawLine( slider->rect().left(), midY,
                       slider->rect().right(), midY);
            
            p.setPen( QColor( 0x3d, 0x3c, 0x3c ));
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
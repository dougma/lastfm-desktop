/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Christian Muehlhaeuser, Last.fm Ltd <chris@last.fm>                *
 *      Erik Jaelevik, Last.fm Ltd <erik@last.fm>                          *
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

#include "volumeslider.h"
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>


VolumeSlider::VolumeSlider( QWidget *parent ) :
        QFrame( parent ),
        m_value( 50 )
{
    m_speakerLow.load( ":/speaker_low.png" );
    m_speakerHigh.load( ":/speaker_high.png" );

    m_sliderKnob.load( ":/slider_knob.png"  );
    m_sliderLeft.load( ":/slider_left.png" );
    m_sliderRight.load( ":/slider_right.png" );
    m_sliderMiddle.load( ":/slider_middle.png" );

    int left = 11 + m_speakerLow.width() + m_sliderLeft.width();
    int right = width() - 10 - m_speakerHigh.width() - m_sliderRight.width();
    m_sliderMiddle = m_sliderMiddle.scaled( right - left, m_sliderMiddle.height(), Qt::IgnoreAspectRatio, Qt::FastTransformation );

    setMinimumHeight( 16 );
    setMinimumWidth( 100 );
}


void
VolumeSlider::setValue( int value )
{
    m_value = qBound( 0, value, 100 );;
    update();

    emit valueChanged( m_value );
}


void
VolumeSlider::paintEvent( QPaintEvent * /*event*/ )
{
    QPainter painter( this );

    // Speaker icons
    painter.drawImage( 7, 0, m_speakerLow );
    painter.drawImage( width() - 7 - m_speakerHigh.width(), 0, m_speakerHigh );

    // Slider
    int left = 11 + m_speakerLow.width();
    int right = width() - 10 - m_speakerHigh.width();
    painter.drawImage( left, 3, m_sliderLeft );
    painter.drawImage( right - 1 - m_sliderRight.width(), 3, m_sliderRight );

    left += m_sliderLeft.width();
    right -= m_sliderRight.width();
    painter.drawImage( left, 3, m_sliderMiddle );

    float pos = (float)( right - left - 2 ) * ( value() / 100.0 ) + 1.0;
    painter.drawImage( left - ( m_sliderKnob.width() / 2 ) + (int)pos, 2, m_sliderKnob );
}


void
VolumeSlider::mouseMoveEvent( QMouseEvent* e )
{
    int left = 11 + m_speakerLow.width() + m_sliderLeft.width();
    int right = width() - 12 - m_speakerHigh.width() - m_sliderRight.width();

    if (e->pos().x() <= 0)
        setValue( 0 );

    else if (e->pos().x() >= right)
        setValue( 100 );

    else {
        float pos = float( e->pos().x() - left ) / float( right - left );
        setValue( int( pos * 100.0 ) );
    }
}


void
VolumeSlider::mousePressEvent( QMouseEvent* e )
{
    if (e->button() == Qt::LeftButton)
        mouseMoveEvent( e );
}


void
VolumeSlider::wheelEvent( QWheelEvent* event )
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;

    setValue( value() + ( 5 * numSteps ) );
}


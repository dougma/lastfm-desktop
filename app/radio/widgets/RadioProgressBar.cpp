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
#include "RadioProgressBar.h"
#include <lastfm/Track>
//#include "widgets/UnicornWidget.h"
//#include "the/radio.h"
//#include <QApplication>
#include <QWidget>
#include <QPaintEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>


RadioProgressBar::RadioProgressBar( QWidget* parent ) 
                : QWidget( parent )
                , m_scrobbleProgressTick( 0 )
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    
    QHBoxLayout* h = new QHBoxLayout( this );
    h->addWidget( ui.time = new QLabel( "00:00") );
    h->addStretch();
    h->addWidget( ui.timeToGo = new QLabel( "00:00"));
    h->setMargin( 0 );
    h->setSpacing( 0 );
    
    ui.time->setAttribute( Qt::WA_MacMiniSize );
    ui.timeToGo->setAttribute( Qt::WA_MacMiniSize );
    
    ui.time->setMargin( 0 );
    ui.timeToGo->setMargin( 0 );
    
#ifndef WIN32
    QFont f = ui.time->font();
    f.setPointSize( 9 );
    ui.time->setFont( f );
    ui.timeToGo->setFont( f );
#endif
    
}
        

void
RadioProgressBar::paintEvent( QPaintEvent* e )
{
    // not a great check for "nothing playing" but works as the code is currently --mxcl
    if (ui.timeToGo->isHidden())
        return;
    
    uint const progressHeight = 10;
    uint const vOffset = (height() / 2) - ( progressHeight / 2 );
    
    uint x1 = ui.time->geometry().right() + 7;
    uint w = width() - ui.time->geometry().width() - ui.timeToGo->geometry().width() - 14;
    {
        QPainter p( this );
        p.setBackgroundMode( Qt::TransparentMode );
        p.fillRect( x1, vOffset + 1, w, progressHeight-2, QColor( 0x16, 0x16, 0x16 ) );
        
        p.setCompositionMode( QPainter::CompositionMode_SourceOver );	
        p.setPen( QColor( 0xe2, 0xe2, 0xe2 ) );
        p.setBrush( Qt::NoBrush );
        if( m_scrobbleProgressTick < m_currentTrackDuration )
            for (uint x = 0, n = ( qreal(m_scrobbleProgressTick) / qreal(m_currentTrackDuration))* (w - 4); x < n; x += 2)
            {
                uint const i = x+x1+2;
                p.drawLine( i, vOffset + 3, i, vOffset + progressHeight-4 );
            }
    }
    QWidget::paintEvent( e );
}
        

QSize
RadioProgressBar::sizeHint() const
{
    QSize s( ui.time->sizeHint().width() + 15 + ui.timeToGo->sizeHint().width(),
                 ui.time->sizeHint().height());
//    qDebug() << "Size Hint: " << s.width();
    return s;
}


void
RadioProgressBar::onRadioTick(qint64 tick )
{
    m_scrobbleProgressTick = tick / 1000;
    QTime t( 0, 0 );
    t = t.addSecs( m_scrobbleProgressTick );
    if( tick > 0 )
        ui.time->setText( t.toString( "mm:ss" ));
    if( m_scrobbleProgressTick > 0 )
        update();
}


void
RadioProgressBar::onTrackSpooled( const Track& track, class StopWatch* )
{
    if( !track.isNull() && track.source() == Track::LastFmRadio )
    {
        m_currentTrackDuration = track.duration();
        QTime t( 0, 0 );
        ui.time->setText( t.toString( "mm:ss" ));
        t = t.addSecs( m_currentTrackDuration );
        ui.timeToGo->setText( t.toString( "mm:ss" ));
        ui.time->setVisible( true );
        ui.timeToGo->setVisible( true );
    }
    else
    {
        m_scrobbleProgressTick = 0;
        
        if( !ui.time->text().isEmpty())
            update();
        
        ui.time->setHidden( true );
        ui.timeToGo->setHidden( true );
        
    }
    update();
}

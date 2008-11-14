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
 
#ifndef RADIO_PROGRESS_BAR_H
#define RADIO_PROGRESS_BAR_H

#include <QApplication>
#include <QWidget>
#include <QPaintEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include "widgets/UnicornWidget.h"
#include "the/radio.h"

class RadioProgressBar : public QWidget
{
    Q_OBJECT
public:
    RadioProgressBar( QWidget* parent = 0 ): QWidget( parent ), m_scrobbleProgressTick( 0 )
    {
        QHBoxLayout* h = new QHBoxLayout( this );
        h->addWidget( ui.time = new QLabel );
        h->addStretch();
        h->addWidget( ui.timeToGo = new QLabel );
        h->setMargin( 0 );
        h->setSpacing( 0 );
        
        UnicornWidget::paintItBlack( ui.time );
        UnicornWidget::paintItBlack( ui.timeToGo );
        ui.time->setAttribute( Qt::WA_MacMiniSize );
        ui.timeToGo->setAttribute( Qt::WA_MacMiniSize );
        
        ui.time->setMargin( 0 );
        ui.timeToGo->setMargin( 0 );
 
        QFont f = ui.time->font();
        f.setPointSize( 9 );
        ui.time->setFont( f );
        ui.timeToGo->setFont( f );

        setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
        
        connect( qApp, SIGNAL( trackSpooled(const Track&, StopWatch* )), this, SLOT( onTrackSpooled( const Track&, StopWatch*)) );
        connect( &The::radio(), SIGNAL(tick(qint64)), this, SLOT( onRadioTick(qint64)));

    }
    
    void paintEvent( QPaintEvent* e )
    {
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
    
    QSize sizeHint() const
    {
        return QSize( ui.time->sizeHint().width() + 114 + ui.timeToGo->sizeHint().width(),
                      ui.time->sizeHint().height());
    }
    
private slots:
    void onRadioTick(qint64 tick )
    {
        m_scrobbleProgressTick = tick / 1000;
        if( m_scrobbleProgressTick > 0 )
        {
            QTime t( 0, 0 );
            t = t.addSecs( m_scrobbleProgressTick );
            ui.time->setText( t.toString( "mm:ss" ));
            update();
        }
        else
            ui.time->clear();
    }
    
    void onTrackSpooled( const Track& track, class StopWatch* watch )
    {
        if( !track.isNull() && track.source() == Track::LastFmRadio )
        {
            m_currentTrackDuration = track.duration();
            QTime t( 0, 0 );
            t = t.addSecs( m_currentTrackDuration );
            if( ui.timeToGo->text() != t.toString( "mm:ss" ));
            {
                ui.timeToGo->setText( t.toString( "mm:ss" ));
                update();
            }
        }
        else
        {
            m_scrobbleProgressTick = 0;

            if( !ui.time->text().isEmpty())
                update();
                
            ui.time->clear();
            ui.timeToGo->clear();
        }
        update();
    }
    

private:
    int m_scrobbleProgressTick;
    int m_currentTrackDuration;
    
    struct {
        QLabel* time;
        QLabel* timeToGo;
    } ui;
};

#endif RADIO_PROGRESS_BAR_H
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

#ifndef FADING_SCROLL_BAR_H
#define FADING_SCROLL_BAR_H

#include <QScrollBar>


#include <QPainter>
#include <QTimeLine>
#include <QTimer>


class FadingScrollBar : public QScrollBar
{
    Q_OBJECT
    
    QTimer* m_hideTimer;
    QTimeLine* m_timeline;
    bool m_hide_when_zero;

public:
    FadingScrollBar( QWidget* parent ) : QScrollBar( parent )
    {
        m_hide_when_zero = false;
        
        m_timeline = new QTimeLine( 300, this );
        m_timeline->setFrameRange( 0, 255 );
        m_timeline->setUpdateInterval( 5 );
        connect( m_timeline, SIGNAL(frameChanged( int )), SLOT(update()) );
        QScrollBar::hide();
        
        m_hideTimer = new QTimer( this );
        m_hideTimer->setSingleShot(true);
        m_hideTimer->setInterval( 1500 );
        connect( m_hideTimer, SIGNAL(timeout()), SLOT(fadeOut()) );
    }
    
public slots:
    void fadeIn()
    {
        m_hideTimer->stop();        
        
        if (isVisible())
            return;
        m_hide_when_zero = false;
        m_timeline->setCurveShape( QTimeLine::EaseInCurve );
        m_timeline->setDirection( QTimeLine::Backward );
        m_timeline->start();
        QScrollBar::show();
    }
    
    void fadeOut()
    {
        m_hideTimer->stop();        
        
        if (isHidden())
            return;
        m_hide_when_zero = true;
        m_timeline->setCurveShape( QTimeLine::EaseOutCurve );
        m_timeline->setDirection( QTimeLine::Forward );
        m_timeline->start();
    }
    
    void fadeOutLater()
    {
        m_hideTimer->start();
    }
    
private:
    virtual void sliderChange( SliderChange change )
    {
        if (change == SliderValueChange && !isVisible())
        {
            fadeIn();
            m_hideTimer->start();
        }
        
        QScrollBar::sliderChange( change );
    }
    
    virtual void paintEvent( QPaintEvent* e )
    {           
        QScrollBar::paintEvent( e );
        
        if (m_timeline->state() == QTimeLine::Running)
        {
            int const f = m_timeline->currentFrame();
            
            QPainter p( this );
            p.fillRect( rect(), QColor( 0x16, 0x16, 0x17, f ) );
            
            if (m_hide_when_zero && f >= 254) 
                QScrollBar::hide();
        }
    }
};

#endif

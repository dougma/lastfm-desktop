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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#ifndef FADING_SCROLL_BAR_H
#define FADING_SCROLL_BAR_H

#include <QScrollBar>
#include <QTimer>


class FadingScrollBar : public QScrollBar
{
    Q_OBJECT
    
    class QTimer* m_hideTimer;
    class QTimeLine* m_timeline;
    bool m_hide_when_zero;

public:
    FadingScrollBar( QWidget* parent );
    
public slots:
    void fadeIn();
    void fadeOut();
    
    void fadeOutLater()
    {
        m_hideTimer->start();
    }
    
private:
    virtual void sliderChange( SliderChange change );
    virtual void paintEvent( QPaintEvent* e );
};

#endif

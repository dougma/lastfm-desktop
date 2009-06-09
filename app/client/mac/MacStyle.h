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
#ifdef Q_WS_MAC
#include <QMacStyle>
#include <QPainter>


class UnicornMacStyle : public QMacStyle
{
    virtual int pixelMetric( PixelMetric metric, const QStyleOption* option, const QWidget* widget ) const
    {
        if (metric == PM_DockWidgetSeparatorExtent)
        {
            return QPixmap( ":/DockWindow/splitter/knob.png" ).height();
        }
        else
            return QMacStyle::pixelMetric( metric, option, widget );
    }
    
    virtual void drawPrimitive( PrimitiveElement element, const QStyleOption* opt, QPainter* p, const QWidget* widget ) const 
    {
        if (element == QStyle::PE_IndicatorDockWidgetResizeHandle)
        {
            if (opt->state & QStyle::State_Horizontal)
            {
                p->drawPixmap( opt->rect, QPixmap( ":/DockWindow/splitter/base.png" ) );
                QPixmap px( ":/DockWindow/splitter/knob.png" );
                int const x = opt->rect.center().x() - px.width()/2;
                p->drawPixmap( x, opt->rect.top(), px );
            }
            else
            {
                p->fillRect( opt->rect, QColor( 35, 35, 35 ) );
                p->setPen( QColor( 24, 23, 23 ) );
                int x = opt->rect.right() - 1;
                int const h = opt->rect.height();
                p->drawLine( x, 0, x, h );
                p->setPen( QColor( 57, 57, 57 ) );
                ++x;
                p->drawLine( x, 0, x, h );                
            }
        }
        else
            QMacStyle::drawPrimitive( element, opt, p, widget );
    }
    
    virtual void drawControl( ControlElement element, const QStyleOption* opt, QPainter* p, const QWidget* widget ) const
    {
        if (element == CE_DockWidgetTitle)
        {
            p->setPen( QColor( 35, 35, 35 ) );
            p->drawRect( opt->rect.adjusted( 0, 1, 0, 0 ) );
            p->drawPixmap( opt->rect, QPixmap(":/DockWindow/title_bar.png") );
            p->setPen( QColor( 54, 53, 53 ) );
            QFont f = p->font();
            f.setBold( false );
            f.setPointSize( 11 );
            p->setFont( f );
            p->drawText( opt->rect, Qt::AlignCenter, widget->windowTitle() );
        }
        else
            QMacStyle::drawControl( element, opt, p, widget );
    }
};

#endif

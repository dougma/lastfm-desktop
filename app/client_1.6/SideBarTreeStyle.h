/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Christian Muehlhaeuser, Last.fm Ltd <chris@last.fm>                *
 *      Max Howell, Last.fm Ltd <max@last.fm>                              *
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

#ifndef TREE_STYLE_H
#define TREE_STYLE_H

#ifndef Q_WS_MAC

#include <QPainter>
#include <QStyleOption>

#ifdef WIN32
    #include <QWindowsXPStyle>
    #include <QWindowsVistaStyle>
#else
    #include <QPlastiqueStyle>
#endif


template <class T> class TreeStyle : public T
{
public:
    virtual void drawPrimitive( QStyle::PrimitiveElement element,
                                const QStyleOption* option,
                                QPainter* painter, 
                                const QWidget* widget) const
    {
        if (element == QStyle::PE_IndicatorBranch)
        {
            static const int decoration_size = 9;
            int mid_h = option->rect.x() + option->rect.width() / 2;
            int mid_v = option->rect.y() + option->rect.height() / 2;
            int bef_h = mid_h;
            int bef_v = mid_v;
            int aft_h = mid_h;
            int aft_v = mid_v;
            if (option->state & QStyle::State_Children)
            {
                int delta = decoration_size / 2;
                bef_h -= delta;
                bef_v -= delta;
                aft_h += delta;
                aft_v += delta;
                painter->setPen(option->palette.dark().color());
                painter->drawLine(bef_h + 2, bef_v + 4, bef_h + 6, bef_v + 4);
                if (!(option->state & QStyle::State_Open))
                {
                    painter->drawLine(bef_h + 4, bef_v + 2, bef_h + 4, bef_v + 6);
                }
                QPen oldPen = painter->pen();
                painter->drawRect(bef_h, bef_v, decoration_size - 1, decoration_size - 1);
                painter->setPen(oldPen);
            }
        }
        else
            T::drawPrimitive(element, option, painter, widget);
    }
};

#endif
#endif

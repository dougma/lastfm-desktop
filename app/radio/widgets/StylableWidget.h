/*
 Copyright 2005-2009 Last.fm Ltd. 
 - Primarily authored by Jono Cole and Doug Mansell

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

#ifndef STYLABLE_WIDGET_H_
#define STYLABLE_WIDGET_H_
#include <QWidget>
#include <QStyleOption>
#include <QPainter>

class StylableWidget: public QWidget {
protected:
    void paintEvent(QPaintEvent*)
    {
        QStyleOption opt;
        opt.init(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }
};

#endif

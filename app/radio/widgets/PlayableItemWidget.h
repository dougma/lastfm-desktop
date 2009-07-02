/*
   Copyright 2005-2009 Last.fm Ltd. 

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

#ifndef PLAYABLE_ITEM_WIDGET_H
#define PLAYABLE_ITEM_WIDGET_H

#include <QPushButton>
#include <lastfm/RadioStation>

class QMouseEvent;

class PlayableItemWidget : public QPushButton
{
    Q_OBJECT;

public:
    PlayableItemWidget(QString stationTitle, const RadioStation& rs);
    PlayableItemWidget(const RadioStation& rs);

signals:
    void startRadio(RadioStation);

private:
//    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);

    void init();

    RadioStation m_rs;
};

#endif

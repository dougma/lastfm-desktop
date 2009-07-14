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

#ifndef SOURCE_ITEM_WIDGET_H
#define SOURCE_ITEM_WIDGET_H

#include <QWidget>

class SourceItemWidget : public QWidget
{
    Q_OBJECT

public:
    SourceItemWidget(const QString& labelText, QLayout* layout);

public slots:
    void onGotImage();

private:
    class QLabel* m_label;
    class QLabel* m_image;
};

#endif

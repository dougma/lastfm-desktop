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

#ifndef COLUMN_LIST_VIEW_H
#define COLUMN_LIST_VIEW_H

#include <QListView>
class QResizeEvent;

// a listview which switches between displayed 1 or 2 columns
// switches to single column when resized width is less than singleColumnWidth
//
class ColumnListView : public QListView
{
    Q_OBJECT

public:
    ColumnListView(int singleColumnWidth, int gridHeightPx);
    int getDisplayedColumns();

signals:
    void columnsChanged();

protected:
    virtual void resizeEvent(QResizeEvent* e);

private:
    int m_displayColumns;   // current display columns
    int m_singleColumnWidth;
    int m_gridHeightPx;
};

#endif

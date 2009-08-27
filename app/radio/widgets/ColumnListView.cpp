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

#include "ColumnListView.h"
#include <QResizeEvent>


ColumnListView::ColumnListView(int singleColumnWidth, int gridHeightPx)
    : m_displayColumns(-1)
    , m_singleColumnWidth(singleColumnWidth)
    , m_gridHeightPx(gridHeightPx)
{
}

int
ColumnListView::getDisplayedColumns()
{
    if (m_displayColumns == -1) {
        m_displayColumns = size().width() < m_singleColumnWidth ? 1 : 2;
    }
    return m_displayColumns;
}

//virtual
void
ColumnListView::resizeEvent(QResizeEvent* e)
{
    if (e->size().width() < m_singleColumnWidth) {
        setGridSize(QSize(e->size().width(), m_gridHeightPx));
        if (m_displayColumns == 2) {
            m_displayColumns = 1;
            emit columnsChanged();
        }
    } else {
        setGridSize(QSize(e->size().width() / 2, m_gridHeightPx));
        if (m_displayColumns == 1) {
            m_displayColumns = 2;
            emit columnsChanged();
        }
    }
}


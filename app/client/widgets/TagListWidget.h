/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Last.fm Ltd <client@last.fm>                                       *
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

#ifndef TAGLISTWIDGET_H
#define TAGLISTWIDGET_H

#include <QTreeWidget>
#include <QMenu>

namespace Tags
{
    enum SortOrder
    {
        MostPopularOrder,
        AscendingOrder,
        DescendingOrder
    };
}


class TagListWidget : public QTreeWidget
{
    Q_OBJECT

public:
    TagListWidget( QWidget* parent = 0 );

    void setSortOrder( Tags::SortOrder sortOrder );
    Tags::SortOrder sortOrder() { return m_sortOrder; }

    void sort();

    QTreeWidgetItem* addItem( QString tag );
    void addItems( const QStringList& labels );

private:
    QMenu m_sortTagsMenu;
    Tags::SortOrder m_sortOrder;

    QAction* m_actionSortMostPopular;
    QAction* m_actionSortAZ;
    QAction* m_actionSortZA;

private slots:
    void showSortContextMenu( const QPoint& point );
    
    void openTagPageForCurrentItem();

    void sortAZ();
    void sortZA();
    void sortMostPopular();
};

#endif // TAGLISTWIDGET_H

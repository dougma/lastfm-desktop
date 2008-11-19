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
 
#ifndef SOURCES_LIST_H
#define SOURCES_LIST_H

#include <QListWidget>

class SourcesList : public QListWidget
{
    Q_OBJECT
    
    friend class Sources;
    
public:
    SourcesList( QWidget* parent );
    
    void refresh()
    {
        if( viewMode() == QListView::IconMode )
            setGridSize( QSize( 70, 70 ) );
        else
            setGridSize( QSize() );
    }

    enum ViewMode {
        ListMode = QListView::ListMode,
        IconMode = QListView::IconMode,
        CustomMode = 1000
    };
    
    ViewMode sourcesViewMode()
    {
        if( !m_customModeEnabled )
            return (ViewMode)QListWidget::viewMode();
        else
            return CustomMode;
    }
    
    void setSourcesViewMode( ViewMode m );
    
    void addCustomWidget( QWidget* w );
    
protected:
    QMimeData* mimeData( const QList<QListWidgetItem *> items ) const;
    
    void focusOutEvent( QFocusEvent* event )
    {
        clearSelection();
        update();
    }
    
private slots:
    void onDataChanged( const QModelIndex& topLeft, const QModelIndex& bottomRight )
    {
        scheduleDelayedItemsLayout();
    }
    
private:
    QMap< QModelIndex, QRect > m_itemRects;
    QSize m_itemSizeHint;
    bool m_customModeEnabled;
    QWidget* m_customWidget;
    
};

#endif //SOURCES_LIST_H

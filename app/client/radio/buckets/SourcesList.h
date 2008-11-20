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
#include <QStyledItemDelegate>
#include <QResizeEvent>

class SourcesList : public QListWidget
{
    Q_OBJECT
    
    friend class Sources;
    
public:
    SourcesList( QWidget* parent );

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
    
    void focusOutEvent( QFocusEvent* )
    {
        clearSelection();
        update();
    }
    
private slots:
    void onDataChanged( const QModelIndex&, const QModelIndex& )
    {
        scheduleDelayedItemsLayout();
    }
    
private:
    QMap< QModelIndex, QRect > m_itemRects;
    QSize m_itemSizeHint;
    bool m_customModeEnabled;
    QWidget* m_customWidget;
    
    class SourcesListDelegate : public QStyledItemDelegate
    {
    public:
        SourcesListDelegate( QObject* parent = 0 ): QStyledItemDelegate( parent ){}
        virtual QSize sizeHint ( const QStyleOptionViewItem& option, const QModelIndex& index ) const
        {
            QStyleOptionViewItemV4 opt = option;
            initStyleOption( &opt, index );
            if( opt.decorationPosition == QStyleOptionViewItem::Left )
            {
                //Add 10px padding when in ListView mode
                return QStyledItemDelegate::sizeHint( option, index ) + QSize( 0, -10 );
            }
            else 
            {
                return QStyledItemDelegate::sizeHint( option, index ) + QSize( 20, 0 );
            }
        }
    };
    
};

#endif //SOURCES_LIST_H

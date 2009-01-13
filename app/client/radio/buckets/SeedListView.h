/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
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
 
#ifndef SEED_LIST_VIEW_H
#define SEED_LIST_VIEW_H

#include <QListWidget>
#include <QStyledItemDelegate>
#include <QResizeEvent>
#include <QAction>
#include <QDebug>
class SeedListView : public QListView
{
    Q_OBJECT
    
    friend class Sources;
    
public:
    SeedListView( QWidget* parent );

    enum ViewMode {
        ListMode = QListView::ListMode,
        IconMode = QListView::IconMode,
        CustomMode = 1000
    };
    
    ViewMode sourcesViewMode()
    {
        if( !m_customModeEnabled )
            return (ViewMode)QListView::viewMode();
        else
            return CustomMode;
    }
    
    void addCustomWidget( QWidget* w, QString title = "" );
    
    void setSourcesViewMode( ViewMode m );
    class SeedListModel* seedModel(){ return m_model; }
    
public slots:
    void showCustomWidget( bool );
    
protected:
    void focusOutEvent( QFocusEvent* )
    {
        clearSelection();
        update();
    }
    
    virtual void showEvent( QShowEvent* );
    virtual void dragMoveEvent( QDragMoveEvent* e )
    {
        e->ignore();
    }
    
    virtual void dropEvent( QDropEvent* e )
    {
        e->ignore();
    }
    
private slots:
    void onDataChanged( const QModelIndex&, const QModelIndex& )
    {
        scheduleDelayedItemsLayout();
    }

    void onViewModeAction( bool );
    
private:
    QMap< QModelIndex, QRect > m_itemRects;
    QSize m_itemSizeHint;
    QWidget* m_customWidget;
    bool m_customModeEnabled;
    class SeedListModel* m_model;
    
    struct {
        struct {
        class QAction* listView;
        class QAction* iconView;
        class QAction* customView;
        } actions;
    } ui;
    
    class SourcesListDelegate : public QStyledItemDelegate
    {
    public:
        SourcesListDelegate( QObject* parent = 0 ): QStyledItemDelegate( parent ){}
        
        virtual void paint( QPainter* p, const QStyleOptionViewItem& option, const QModelIndex& i ) const
        {
            QStyleOptionViewItem o = option;
            o.decorationSize = ( o.decorationPosition == QStyleOptionViewItem::Top ) ? m_iconModeSize : m_listModeSize;
            return QStyledItemDelegate::paint( p, o, i );
        }
        
        virtual QSize sizeHint( const QStyleOptionViewItem& option, const QModelIndex& i ) const
        {
            QStyleOptionViewItem o = option;
            o.decorationSize = ( o.decorationPosition == QStyleOptionViewItem::Top ) ? m_iconModeSize : m_listModeSize;
            return QStyledItemDelegate::sizeHint( o, i );
        }

        void setIconSize( const QListView::ViewMode m, QSize s )
        {
            switch( m )
            {
                case QListView::IconMode: m_iconModeSize = s; break;
                case QListView::ListMode: m_listModeSize = s; break;
            }
        }
        
    private:
        QSize m_iconModeSize;
        QSize m_listModeSize;
    };
    
};

#endif //SEED_LIST_VIEW_H

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

#ifndef SIDE_BAR_TREE_H
#define SIDE_BAR_TREE_H

#include "SideBarRevealPopup.h"
#include "SideBarModel.h"
#include "SideBarDelegate.h"

#include <QPointer>
#include <QTreeView>


class SideBarTree : public QTreeView
{
    Q_OBJECT

    public:
        SideBarTree( QWidget* parent = 0 );

    //////
        void addRecentlyPlayedTrack( Track );

    signals:
        void statusMessage( const QString& message );
        void plsShowRestState();
        void plsShowNowPlaying();

    private slots:
        void expandIndexUnderMouse();
        void onActivated( const QModelIndex& );
        void onContainerPageChanged( int );
//TODO        void onAppEvent( int event, const QVariant& data );

    protected:
        virtual void currentChanged( const QModelIndex& current, const QModelIndex& previous );
        virtual void contextMenuEvent( QContextMenuEvent* );
        virtual void dropEvent( QDropEvent* );
        virtual void dragMoveEvent( QDragMoveEvent* );
        virtual void dragLeaveEvent( QDragLeaveEvent* );
        virtual void mouseMoveEvent( QMouseEvent* );
        virtual void drawRow( QPainter*, const QStyleOptionViewItem&, const QModelIndex& ) const;
        virtual void mousePressEvent( QMouseEvent* );
        virtual void scrollContentsBy( int dx, int dy );
        virtual void showEvent( QShowEvent* );

        #ifdef HIDE_RADIO
        virtual void reset();
        #endif
        #ifdef Q_WS_MAC
        virtual void drawBranches( QPainter*, const QRect&, const QModelIndex& ) const;
        #endif

    private:
        enum ContextMenuActionType { ExecQMenu, DoQMenuDefaultAction };

        void contextMenuHandler( const QModelIndex&, ContextMenuActionType );
        void dragDropHandler( class QDropEvent* );
        bool dragDropHandlerPrivate( const QModelIndex&, QDropEvent*, QString& status_message );
        
        void makeWebRequests();

        QTimer* m_timer;
        SideBarModel* m_model;
        SideBarDelegate* m_delegate;
        QPointer<RevealPopup> m_revealer;
        ToolTipLabel* m_drag_tip;
        bool m_empty_model;
};

#endif

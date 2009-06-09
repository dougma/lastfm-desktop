/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

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
#ifndef UNICORN_TAB_WIDGET_H
#define UNICORN_TAB_WIDGET_H

#include <QPixmap>
#include <QStackedWidget>
#include <QTabBar>

#include <QDebug>
namespace Unicorn
{
    class TabBar : public QTabBar
    {
        friend class UnicornMacStyle;
        
    public:
        TabBar();
        
        virtual QSize sizeHint() const;
        void setSpacing( int );
        void setTearable( bool t ){ m_tearable = t; }
        void addWidget( QWidget* w );
        
    protected:
        virtual void mousePressEvent( QMouseEvent* );
        virtual void mouseReleaseEvent( QMouseEvent* );
        virtual void mouseMoveEvent( QMouseEvent* );
        virtual void paintEvent( QPaintEvent* );
        virtual void tabInserted( int );
        virtual void tabRemoved( int );
        virtual bool eventFilter(QObject* , QEvent* );
        
        int m_spacing;
        const int m_leftMargin;
        const QPixmap m_active;
        
    private:
        QPoint m_mouseDownPos;
        bool m_tearable;
        static const uint k_startTearDistance;
    };
    
    
    class TabWidget : public QWidget
    {
        Q_OBJECT
        QStackedWidget* m_stack;
        TabBar* m_bar;
        
    public:
        TabWidget();
        
        TabBar* bar() const { return m_bar; }
        
        void addTab( const QString& title, QWidget* );
        void addTab( QWidget* );
        QWidget* currentWidget() const { return m_stack->currentWidget(); }
        virtual QSize sizeHint() const { return QWidget::sizeHint().expandedTo( m_bar->sizeHint() ); }
        
        void setTabEnabled( int index, bool );
        void setTearable( bool t ){ bar()->setTearable( t ); }
        QWidget* widget( int index ) const;

    signals:
        void currentChanged( int );

    };
}

#endif

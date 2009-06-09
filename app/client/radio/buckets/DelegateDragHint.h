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
#ifndef DELEGATE_DRAG_HINT_H
#define DELEGATE_DRAG_HINT_H

#include <QAbstractItemDelegate>
#include <QPainter>
#include <QPaintEvent>
#include <QTimeLine>
#include <QDebug>

class DelegateDragHint : public QWidget
{
    Q_OBJECT
public:
    DelegateDragHint( QAbstractItemDelegate* d, const QModelIndex& i, const QStyleOptionViewItem& options, QWidget* p ): QWidget( 0 ), m_mimeData( 0 ), m_d( d ), m_i( i ), m_options( options )
    {
        setPalette( p->palette() );
        setFocusPolicy( Qt::NoFocus );
        setWindowFlags( Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint );
        setWindowOpacity( 0.3 );
        resize( options.rect.size() );
        move( p->mapToGlobal( options.rect.topLeft()));
    }
    
    void paintEvent( QPaintEvent* e );
    
    void dragTo( QWidget* target );
    
    template<typename T> 
    inline void dragToChild( QWidget* target )
    {
        QWidget* finalTarget = target->findChild<T>();
        
        if( !finalTarget )
            return;
        
        dragTo( finalTarget );
    }
   
    void setMimeData( QMimeData* data ){ m_mimeData = data; }
    
    QSize sizeHint() const { QStyleOptionViewItem option; option.rect = rect(); return m_d->sizeHint( option, m_i); }
    
    QModelIndex index(){ return m_i; }
    
signals:
    void finishedAnimation();
    
private slots:
    void onDragFrameChanged( int frame );
    
    void onFinishedAnimation();
    
private:
    QWidget* m_target;
    QPoint m_startPoint;
    QMimeData* m_mimeData;
    QAbstractItemDelegate* m_d;
    QModelIndex m_i;
    QStyleOptionViewItem m_options;
};

#endif //DELEGATE_DRAG_HINT_H
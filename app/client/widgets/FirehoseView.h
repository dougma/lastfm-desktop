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

#ifndef FIREHOSE_VIEW_H
#define FIREHOSE_VIEW_H

#include <QAbstractScrollArea>
#include <QModelIndex>

/** view that can only do minimal things, hence we don't even derive
  * QAbstractItemView as that was way more work than we required 
  * 
  * @author <max@last.fm> 
  */
class FirehoseView : public QAbstractScrollArea
{
    Q_OBJECT

    class QTimeLine* timer;    
    
    int h;
    int offset;    
    
protected:
    class QAbstractItemDelegate* delegate;
    class QAbstractItemModel* model;

public:
    FirehoseView();

    /** you can set any model you like, but it prolly won't work as expected,
      * also, if you don't set this, we crash :P */
    void setModel( QAbstractItemModel* );
    /** as above */
    void setDelegate( QAbstractItemDelegate* );

private slots:
    void onRowInserted();
    void onModelReset();
    void onFrameChange( int );

protected:
    virtual void paintEvent( QPaintEvent* );
    virtual void resizeEvent( QResizeEvent* );
    virtual void scrollContentsBy( int, int );
    
    virtual void mouseDoubleClickEvent( QMouseEvent* );
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent( QMouseEvent* event );
    
    QModelIndex indexAt( const QPoint& point ) const;
    QRect visualRect( const QModelIndex& i ) const;
    
    QScrollBar* bar() const { return verticalScrollBar(); }
    
private:
    QMap< QModelIndex, QRect > m_itemRects;
    QPoint m_dragStartPosition;
};

#endif

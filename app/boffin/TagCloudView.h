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

#ifndef TAG_CLOUD_VIEW_H
#define TAG_CLOUD_VIEW_H

#include <QAbstractItemView>
#include "BoffinTagRequest.h"

class TagCloudView : public QAbstractItemView
{
    Q_OBJECT
public:
    TagCloudView( QWidget* parent = 0 );

    // QAbstractItemView pure virtual functions:
    virtual QModelIndex indexAt( const QPoint& ) const;
    virtual void scrollTo( const QModelIndex&, ScrollHint ) {};
    virtual QRect visualRect( const QModelIndex& ) const;

    void setModel(QAbstractItemModel *model);
    QStringList currentTags() const;

public slots:
    virtual void selectAll();

protected slots:
    virtual void updateGeometries();
    void onRows(const QModelIndex & parent, int start, int end);
    void onFetchedTags();
    void onTag( const BoffinTagItem& );

protected:
    void rectcalc();

    virtual void paintEvent( QPaintEvent* );
    virtual bool isIndexHidden( const QModelIndex& ) const{ return false; }
    virtual void setSelection( const QRect&, QItemSelectionModel ){};
    virtual QRegion visualRegionForSelection( const QItemSelection& ) const{ return QRegion(); }
    virtual QModelIndex moveCursor( CursorAction, Qt::KeyboardModifiers ) { return QModelIndex(); }
    virtual int horizontalOffset() const{ return 0; }
    virtual int verticalOffset() const{ return 0; }
    virtual void setSelection( const QRect&, QItemSelectionModel::SelectionFlags );

    virtual bool viewportEvent(QEvent *event);

    QModelIndex m_hoverIndex;
    QHash<int, QRect> m_rects;      // row -> QRect

    typedef QHash<int, QRect>::const_iterator RectsConstIt;
    typedef QHash<int, QRect>::iterator RectsIt;

    bool m_dirty;
    bool m_fetched;
    QString m_loadedTag;
};

#endif //TAG_CLOUD_VIEW_H

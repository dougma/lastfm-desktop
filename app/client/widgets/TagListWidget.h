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


class TagListWidget : public QTreeWidget
{
    Q_OBJECT

public:
    TagListWidget( QWidget* parent = 0 );
    
    void add( const QString& );
    void setTagsRequest( class WsReply* );

private slots:
    void onTagsRequestFinished( WsReply* );
    
private:
    class QMenu* m_menu;
 
    virtual int sizeHintForRow( int r ) const { return QTreeWidget::sizeHintForRow( r ) + 10; }
    
private slots:
    void showMenu( const QPoint& );
    
    void sortByPopularity();
    void sortAlphabetically();
    void openTagPageForCurrentItem();
};


class TagIconView : public TagListWidget
{
    virtual void paintEvent( QPaintEvent* );

public:
    TagIconView();
};

#endif // TAGLISTWIDGET_H

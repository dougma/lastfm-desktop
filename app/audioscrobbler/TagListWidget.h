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
#ifndef TAGLISTWIDGET_H
#define TAGLISTWIDGET_H

#include <QTreeWidget>
#include "PlayableMimeData.h"


class TagListWidget : public QTreeWidget
{
    Q_OBJECT

public:
    TagListWidget( QWidget* parent = 0 );
    
    using QTreeWidget::indexFromItem;
    
    /** we won't add the tag if we already have it, and in that case we 
      * return false */
    bool add( QString );
    QStringList newTags() const { return m_newTags; }

public slots:
    void setTagsRequest( QNetworkReply* );
    
protected:
    virtual QMimeData* mimeData( const QList<QTreeWidgetItem *> items ) const;
    
private slots:
    void onTagsRequestFinished();
    
private:
    class QMenu* m_menu;
    QStringList m_newTags;
	QNetworkReply *m_currentReply;

    QTreeWidgetItem* createNewItem( QString tag );

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

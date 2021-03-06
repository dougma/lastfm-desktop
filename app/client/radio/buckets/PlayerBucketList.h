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
#ifndef PLAYER_BUCKET_LIST_H
#define PLAYER_BUCKET_LIST_H

#include <QDebug>
#include <QListWidget>
#include <QDropEvent>
#include <QInputDialog>

#include <QDomElement>
#include <QDomNodeList>

#include "the/app.h"
#include <lastfm/Artist.h"
#include "PlayableMimeData>
#include "app/moose.h"



class PlayerBucketList : public QListView
{
	Q_OBJECT
public:
	PlayerBucketList( QWidget* w );
	
	QRect visualRect ( const QModelIndex & index ) const;
	QModelIndex indexAt( const QPoint& point ) const;
    
    bool addItem( class Seed* item );
    
signals:
    void itemRemoved( QString, Seed::Type );
    void itemAdded( QString, Seed::Type );
	
public slots:
    void play();
    
protected:

	void resizeEvent ( QResizeEvent* event );	
	void dropEvent( QDropEvent* event);	
	void dragEnterEvent ( QDragEnterEvent * event );

    void dragMoveEvent( QDragMoveEvent* event ){ event->acceptProposedAction(); }
	
	void paintEvent( QPaintEvent* );
    void scrollContentsBy( int dx, int dy );
    
    Qt::DropActions supportedDropActions () const { return (Qt::CopyAction | Qt::MoveAction | Qt::LinkAction); }
    
    void calculateLayout();
    void calculateItemRemoveLayout();
    void calculateToolIconsLayout();
    
    bool addFromMimeData( const QMimeData* data );

    void addItem( QListWidgetItem* item );
    
    void removeIndex( const QModelIndex& i );
    void removeItem( Seed* item );
    
private slots:
    void onCurrentItemChanged(QListWidgetItem*, QListWidgetItem*);
    void showQuery();
    void clearItems();   
    void removeCurrentItem();
    
    void onQueryEditReturn();
   
    
private:
    static const int k_itemMargin;
    static const int k_itemSizeX;
    static const int k_itemSizeY;
    
    QString m_descriptionText;
    
	bool m_showDropText;
	QMap< QModelIndex, QRect > m_itemRects;
	class WsAccessManager* m_networkManager;

    QString queryString() const;
	QString queryString( const QModelIndex i, bool joined = true ) const;
    
    class SeedListModel* m_model;
    
    struct {
        class ImageButton* clearButton;
        class ImageButton* removeButton;
        class ImageButton* queryEditButton;
        class QLineEdit* queryEdit;
    } ui;

};

#endif //PLAYER_BUCKET_LIST_H

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
 
#ifndef PRIMARY_BUCKET_H
#define PRIMARY_BUCKET_H

#include <QMainWindow>

namespace Unicorn{ class TabWidget; }

class PrimaryBucket : public QMainWindow
{
	Q_OBJECT
public:
	PrimaryBucket();
    

	struct {
		class Unicorn::TabWidget* tabWidget;
        class PrimaryListView* stationsBucket;
		class PrimaryListView* friendsBucket;
		class PrimaryListView* tagsBucket;
		class PlayerBucket* playerBucket;
        class RadioMiniControls* controls;
	} ui;
    
private:
	
private slots:
    void onUserGetFriendsReturn( class WsReply* );
    void onUserGetTopTagsReturn( class WsReply* );
    void onItemDoubleClicked( const class QModelIndex& index );
    void onDnDAnimationFinished();
    
public slots:
	void replaceStation( class QMimeData* );

};


#include <QMouseEvent>
#include <QModelIndex>
#include <QDebug>
#include <QListWidget>
#include "PlayableListItem.h"
#include "PlayableMimeData.h"

class PrimaryListView : public QListWidget
{
    Q_OBJECT
    
    friend class PrimaryBucket;
public:
	PrimaryListView( QWidget* parent ): QListWidget( parent ){};
    
protected:
    QMimeData* mimeData( const QList<QListWidgetItem *> items ) const
    {
        if( items.isEmpty() )
            return 0;

        PlayableListItem* item = dynamic_cast<PlayableListItem*>( items.first() );
        
        PlayableMimeData* data = new PlayableMimeData();
        data->setType( item->playableType() );
        data->setText( item->text() );
        data->setImageData( item->data( Qt::DecorationRole).value<QIcon>().pixmap(50).toImage() );
        return data;
    }

//	void mousePressEvent(QMouseEvent *event)
//	{
//		QModelIndex i = indexAt( event->pos());
//		if (event->button() == Qt::LeftButton
//			&& i.isValid() && i.flags() & Qt::ItemIsDragEnabled ) 
//		{
//			
//			QString text = i.data( Qt::DisplayRole ).toString();
//			QIcon icon = i.data( Qt::DecorationRole ).value<QIcon>();
//			QDrag *drag = new QDrag( this );
//			PlayableMimeData *mimeData = new PlayableMimeData;
//			
//			mimeData->setText( text );
//			mimeData->setImageData( icon.pixmap( 50 ).toImage() );
//			mimeData->setType( i.data( Qt::UserRole ).toInt() );
//			
//			drag->setMimeData( mimeData );
//			drag->setPixmap(icon.pixmap(50 ));
//			drag->setHotSpot(QPoint(drag->pixmap().width()/2,
//									drag->pixmap().height()));
//			
//			Qt::DropAction dropAction = drag->exec( Qt::CopyAction );
//
//			if( dropAction != Qt::IgnoreAction )
//				itemFromIndex( i )->setFlags( Qt::NoItemFlags );
//		}
//	}
	
};

#endif
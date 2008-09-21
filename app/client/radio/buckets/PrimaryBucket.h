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
#include <QListWidget>

class PrimaryBucket : public QMainWindow
{
	Q_OBJECT
public:
	PrimaryBucket( QWidget* w = 0 );

private:
	
	struct {
		class QTabWidget* tabWidget;
		class PrimaryListView* friendsBucket;
		class PrimaryListView* tagsBucket;
	} ui;

};


#include <QMouseEvent>
#include <QModelIndex>
#include <QDebug>
class PrimaryListView : public QListWidget
{
	Q_OBJECT
public:
	PrimaryListView( QWidget* parent ): QListWidget( parent ){};

	void mousePressEvent(QMouseEvent *event)
	{
		QModelIndex i = indexAt( event->pos());
		if (event->button() == Qt::LeftButton
			&& i.isValid() && i.flags() & Qt::ItemIsDragEnabled ) 
		{
			
			QString text = i.data( Qt::DisplayRole ).toString();
			QIcon icon = i.data( Qt::DecorationRole ).value<QIcon>();
			QDrag *drag = new QDrag( this );
			QMimeData *mimeData = new QMimeData;
			
			mimeData->setText( text );
			mimeData->setImageData( icon.pixmap( 50 ).toImage() );
			
			drag->setMimeData( mimeData );
			drag->setPixmap(icon.pixmap(50 ));
			drag->setHotSpot(QPoint(drag->pixmap().width()/2,
									drag->pixmap().height()));
			
			Qt::DropAction dropAction = drag->exec( Qt::CopyAction );

			if( dropAction != Qt::IgnoreAction )
				itemFromIndex( i )->setFlags( Qt::NoItemFlags );
		}
	}
	
};

#endif
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
 
#ifndef PLAYER_BUCKET_H
#define PLAYER_BUCKET_H

#include <QDebug>
#include <QListWidget>
#include <QDropEvent>
#include <QInputDialog>

#include <QDomElement>
#include <QDomNodeList>

#include "the/App.h"
#include "lib/lastfm/types/Artist.h"
#include "PlayableMimeData.h"

class PlayerBucket : public QListWidget
{
	Q_OBJECT
public:
	PlayerBucket( QWidget* w );
	
	QRect visualRect ( const QModelIndex & index ) const;
	QModelIndex indexAt( const QPoint& point ) const;
    
    /** add the item to the bucket and load any associated data (ie image) */
    void addAndLoadItem( const QString& item, const PlayableMimeData::Type );
	
public slots:
    void play();
    
protected:

	void resizeEvent ( QResizeEvent* event );	
	void dropEvent( QDropEvent* event);	
	void dragEnterEvent ( QDragEnterEvent * event );
    
    bool event( QEvent* event )
    {
        switch( event->type() )
        {
            case QEvent::DragEnter:
                dragEnterEvent( static_cast<QDragEnterEvent*>(event) );
                break;
                
            case QEvent::Drop:
                dropEvent( static_cast<QDropEvent*>(event) );
                break;
                
            case QEvent::DragMove:
                dragMoveEvent( static_cast<QDragMoveEvent*>(event) );
                break;

            default:
                return QListWidget::event( event );
        }
        return true;
    }

    //Not really sure why this has to be overloaded but seems to fix dnd on win32
    //qt drag and drop API is NOT very clearly defined :(
    void dragMoveEvent( QDragMoveEvent* event ){ event->acceptProposedAction(); }
	
	void paintEvent( QPaintEvent* );
    void scrollContentsBy( int dx, int dy );
    
    Qt::DropActions supportedDropActions () const { return (Qt::CopyAction | Qt::MoveAction | Qt::LinkAction); }
    
    void calculateLayout();
    
    bool addFromMimeData( const QMimeData* data );
    
   
private:
	static const QString k_dropText;
    static const int k_itemMargin;
    static const int k_itemSizeX;
    static const int k_itemSizeY;
    
	bool m_showDropText;
	QMap< QModelIndex, QRect > m_itemRects;
	class WsAccessManager* m_networkManager;
	
	QString queryString( const QModelIndex i, bool joined = true ) const;

};

#include <QAbstractItemDelegate>
#include <QPainter>
class PlayerBucketDelegate : public QAbstractItemDelegate
{
	Q_OBJECT
public:
	PlayerBucketDelegate( QObject* parent = 0 ):QAbstractItemDelegate( parent ){};
	
	void paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
	{
        if( option.state & QStyle::State_Active )
            painter->fillRect( option.rect, QColor( 0x2e, 0x2e, 0x7e, 0x77) );
        else
            painter->fillRect( option.rect, QColor( 0x2e, 0x2e, 0x2e, 0x77) );
		
		QRect iconRect = option.rect;
		iconRect.adjust( 2, 2, -2, -15 );
		
		QIcon icon = index.data( Qt::DecorationRole ).value<QIcon>();
		icon.paint( painter, iconRect );
		
		painter->setPen( Qt::white );
		QRect textRect = option.rect.adjusted( 5, 60, -5, -5 );
        
		QString text = index.data( Qt::DisplayRole ).toString();
		painter->drawText( textRect, Qt::AlignCenter , text);
	}
	
	QSize sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const
	{
		Q_UNUSED( index );
		return option.rect.size();
	}
    
};

#endif //PLAYER_BUCKET_H
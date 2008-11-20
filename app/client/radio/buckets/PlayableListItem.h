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
 
#ifndef PLAYABLE_LIST_ITEM
#define PLAYABLE_LIST_ITEM

#include <QListWidgetItem>
#include "PlayableMimeData.h"
#include <QNetworkReply>
#include "lib/lastfm/ws/WsAccessManager.h"
#include "app/moose.h"

class PlayableListItem : public QObject, public QListWidgetItem
{
    Q_OBJECT
    
public:
	PlayableListItem( QListWidget* parent = 0 ) : QObject( parent ), QListWidgetItem( parent, QListWidgetItem::UserType ){ setupItem(); m_networkManager = new WsAccessManager( this ); }
	PlayableListItem( const QString & text, QListWidget * parent = 0 )
    :QObject( parent ), QListWidgetItem( text, parent, QListWidgetItem::UserType ){ setupItem(); m_networkManager = new WsAccessManager( this ); } ;
	
    void setupItem()
    {
        QFont f = font();
        f.setPointSize( 10 );
        setFont( f );
        setIcon( QPixmap( 36, 38 ) );
        
        setSizeHint( QSize( 60, 60) );
    }
    
	static PlayableListItem* createFromMimeData( const PlayableMimeData* data, QListWidget* parent = 0 );
	
	void setPlayableType( const Seed::Type t ){ setData( moose::TypeRole, t ); }
    
    Qt::ItemFlags flags() const{ return Qt::ItemIsDragEnabled; }
	
    Seed::Type playableType() const{ return (Seed::Type)data( moose::TypeRole ).toInt(); }
    QString rql() const{ return m_rql; }
    
    void fetchImage();
    
    void setPixmap( const QPixmap );
    
    void setRQL( const QString& rql ){ m_rql = rql; }
    
    void flash();
    
public slots:
    void iconDataDownloaded();
    
private slots:
    void onArtistSearchFinished( WsReply* r );
    void onFlashFrameChanged( int );
    void onFlashFinished();
    
private:
    QPixmap cropToSize( const QPixmap, const QSize& ) const;
    QPixmap overlayPixmap( const QPixmap source, const QPixmap overlay, const QPoint offset = QPoint( 0, 0)) const;
    class WsAccessManager* m_networkManager;
    QString m_rql;
    
};

#endif //PLAYABLE_LIST_ITEM

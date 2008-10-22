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

class PlayableListItem : public QObject, public QListWidgetItem
{
    Q_OBJECT
    
public:
	PlayableListItem( QListWidget* parent = 0 ) : QObject( parent ), QListWidgetItem( parent, QListWidgetItem::UserType ){ m_networkManager = new WsAccessManager( this ); }
	PlayableListItem( const QString & text, QListWidget * parent = 0 )
					:QObject( parent ), QListWidgetItem( text, parent, QListWidgetItem::UserType ){};
	
	static PlayableListItem* createFromMimeData( const PlayableMimeData* data, QListWidget* parent = 0 );
	
	void setType( const PlayableMimeData::Type t ){ setData( k_playableType, t ); }
    
    Qt::ItemFlags flags() const{ return Qt::ItemIsDragEnabled; }
	
	int playableType() const{ return data( k_playableType ).toInt(); }
    
    void fetchImage();
    
    void setIcon( const QIcon& icon );
    
public slots:
    void iconDataDownloaded();
    
private slots:
    void onArtistSearchFinished( WsReply* r );
	
private:
	static const Qt::ItemDataRole k_playableType = Qt::UserRole;
    class WsAccessManager* m_networkManager;
};

#endif //PLAYABLE_LIST_ITEM

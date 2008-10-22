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

#include "PlayableListItem.h"
#include "lib/lastfm/ws/WsReply.h"

PlayableListItem* /* static */
PlayableListItem::createFromMimeData( const PlayableMimeData* data, QListWidget* parent )
{
    PlayableListItem* item = new PlayableListItem( parent );
    
    item->setText( data->text() );
    
    if( data->hasImage() )
        item->setIcon( QIcon( QPixmap::fromImage( data->imageData().value<QImage>())) );
    
    item->setData( k_playableType, data->type() );
    
    return item;
};


void 
PlayableListItem::iconDataDownloaded()
{
    QNetworkReply* reply = static_cast< QNetworkReply* >( sender());
    
    QPixmap pixmap;
    pixmap.loadFromData( reply->readAll() );
    
    setIcon( QIcon( pixmap ) );

}


void 
PlayableListItem::fetchImage()
{
    switch( playableType())
    {
        case PlayableMimeData::ArtistType:
        {
            Artist a( text() );
            WsReply* reply = a.search( 5 );
            
            connect( reply, SIGNAL(finished(WsReply*)), SLOT(onArtistSearchFinished(WsReply*)) );
            return;
        }
            
        case PlayableMimeData::UserType:
        {
            //TODO user.search method - waiting on webteam
            return;
        }
        
        case PlayableMimeData::TagType:
        {
            setIcon( QIcon( ":/buckets/tag_white_on_blue.png" ) );
            return;
        }
    }
}


void 
PlayableListItem::onArtistSearchFinished( WsReply* r )
{
    try
    {
        QList<Artist> results = Artist::search( r );
        
        Artist a = results.first();
        
        if(a != text())
        {
            //TODO: handle exact artist not found case
            //      I'm going to work on the updated player bucket 
            //      before I dive into this.
        }
        
        if( a.imageUrl().isValid() )
        {
            QNetworkReply* get = m_networkManager->get( QNetworkRequest( a.imageUrl() ));
            connect( get, SIGNAL( finished()), SLOT( iconDataDownloaded()));
        }
    }
    catch( CoreDomElement::Exception e )
    {
        return;
    }
}


void 
PlayableListItem::setIcon( const QIcon& icon )
{
    QListWidgetItem::setIcon( icon );
    listWidget()->viewport()->update();
}
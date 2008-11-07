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
#include <QTimeLine>
#include <QPainter>
#include "app/moose.h"


PlayableListItem* /* static */
PlayableListItem::createFromMimeData( const PlayableMimeData* data, QListWidget* parent )
{
    PlayableListItem* item = new PlayableListItem( parent );
    
    item->setText( data->text() );
    
    if( data->hasImage() )
        item->setIcon( QIcon( QPixmap::fromImage( data->imageData().value<QImage>())) );
    
    item->setData( moose::TypeRole, data->type() );
    
    return item;
};


void 
PlayableListItem::iconDataDownloaded()
{
    QNetworkReply* reply = static_cast< QNetworkReply* >( sender());
    
    QPixmap pixmap;
    pixmap.loadFromData( reply->readAll() );
    
    setPixmap( pixmap );
}


void 
PlayableListItem::fetchImage()
{
    switch( playableType())
    {
        case Seed::ArtistType:
        {
            Artist a( text() );
            WsReply* reply = a.search( 5 );
            
            connect( reply, SIGNAL(finished(WsReply*)), SLOT(onArtistSearchFinished(WsReply*)) );
            return;
        }
            
        case Seed::UserType:
        {
            //TODO user.search method - waiting on webteam
            return;
        }
        
        case Seed::TagType:
        {
            setIcon( QIcon( ":/buckets/tag.png" ) );
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
        
        if(((QString)a).toLower() != text().toLower())
        {
            //TODO: handle exact artist not found case
            //      I'm going to work on the updated player bucket 
            //      before I dive into this.
            return;
        }
        
        if( a.imageUrl().isValid() )
        {
            QNetworkReply* get = m_networkManager->get( QNetworkRequest( a.smallImageUrl() ));
            connect( get, SIGNAL( finished()), SLOT( iconDataDownloaded()));
        }
    }
    catch( CoreDomElement::Exception e )
    {
        return;
    }
}


void 
PlayableListItem::setPixmap( const QPixmap& pm )
{
    QPixmap iconPm = pm;
    QSize diff = pm.size() - QSize( 34, 34 );

    //crop the avatar to a 34x34 square
    if( diff.height() > 0 || diff.width() > 0 )
    {
        const QPixmap scaled = pm.scaled( 34, 34, Qt::KeepAspectRatioByExpanding );
        const QPixmap cropped = scaled.copy( ((scaled.width() - 34) / 2), ((scaled.height() - 34) / 2), 34, 34);
        iconPm = cropped;
    }
    
    setIcon( iconPm );
    
    if( listWidget())
        listWidget()->viewport()->update();
}


void 
PlayableListItem::flash()
{
    QTimeLine* flashTimeLine = new QTimeLine( 150, this );
    flashTimeLine->setFrameRange( 0, 100 );
    flashTimeLine->setCurveShape( QTimeLine::EaseOutCurve );
    connect( flashTimeLine, SIGNAL( frameChanged( int )), SLOT( onFlashFrameChanged( int )));
    connect( flashTimeLine, SIGNAL( finished()), SLOT( onFlashFinished()));
    flashTimeLine->start();
}


void 
PlayableListItem::onFlashFrameChanged( int frame )
{
    setData( moose::HighlightRole, QVariant::fromValue<int>( frame ) );
    if( listWidget() )
        listWidget()->viewport()->update();
    
}


void 
PlayableListItem::onFlashFinished()
{
    QTimeLine* tl = static_cast<QTimeLine*>(sender());
    
    if( tl->direction() == QTimeLine::Forward )
    {
        tl->setCurveShape( QTimeLine::EaseInCurve );
        tl->setDuration( 300 );
        tl->setDirection( QTimeLine::Backward );
        tl->start();
    }
    else
        tl->deleteLater();
    
}

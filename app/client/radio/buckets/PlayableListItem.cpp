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
    
    item->setData( moose::TypeRole, data->type() );
    
    if( data->hasImage() )
        item->setIcon( QIcon( QPixmap::fromImage( data->imageData().value<QImage>())) );
    else
        item->fetchImage();
    
    return item;
};


void 
PlayableListItem::iconDataDownloaded()
{
    QNetworkReply* reply = static_cast< QNetworkReply* >( sender());
    
    QPixmap pixmap;
    pixmap.loadFromData( reply->readAll() );
    
    if( pixmap.isNull() )
        return;
    
    setPixmap( pixmap );
    const QRect textRect = listWidget()->fontMetrics().boundingRect( text() );
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
            setIcon( QIcon( ":/buckets/tag_39.png" ) );
            return;
        }
    }
}


void 
PlayableListItem::onArtistSearchFinished( WsReply* r )
{
    try
    {
        QList<Artist> results = Artist::list( r );
        
        if( results.isEmpty() )
            return;
        
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
PlayableListItem::setPixmap( const QPixmap pm )
{
    const QPixmap pm34 = cropToSize( pm, QSize( 34, 34 ));
    const QPixmap pm17 = cropToSize( pm, QSize( 17, 17 ));
    
    const QPixmap overlayedIcon34 = overlayPixmap( pm34, QPixmap( ":buckets/avatar_overlay_34.png" ), QPoint( 1, 1 ));
    const QPixmap overlayedIcon17 = overlayPixmap( pm17, QPixmap( ":buckets/avatar_overlay_17.png" ), QPoint( 1, 1 ));
    
    QPixmap selectedIcon34 = overlayedIcon34;
    {
        QRect selectRect = overlayedIcon34.rect().adjusted( 1, 1, -1, -3 );
        QPainter p( &selectedIcon34 );
        p.drawPixmap( selectRect, QPixmap( ":buckets/avatar_overlay_selected.png" ) );
    }

    QPixmap selectedIcon17 = overlayedIcon17;
    {
        QRect selectRect = overlayedIcon17.rect().adjusted( 1, 1, -1, -3 );
        QPainter p( &selectedIcon17 );
        p.drawPixmap( selectRect, QPixmap( ":buckets/avatar_overlay_selected.png" ) );
    }
    
    QIcon icon;
    icon.addPixmap( overlayedIcon34 );
    icon.addPixmap( selectedIcon34, QIcon::Selected );
    icon.addPixmap( overlayedIcon17 );
    icon.addPixmap( selectedIcon17, QIcon::Selected );
    setIcon( icon );
}


QPixmap 
PlayableListItem::overlayPixmap( const QPixmap source, const QPixmap overlay, const QPoint offset ) const
{
    QPixmap output( source.size().expandedTo( overlay.size()));

    QRect iRect = source.rect().translated( offset );
    output.fill( Qt::transparent );
    QPainter p( &output );
    p.setRenderHint( QPainter::SmoothPixmapTransform );
    p.drawPixmap( iRect, source );
    p.drawPixmap( output.rect(), overlay);

    return output;
}


QPixmap 
PlayableListItem::cropToSize( const QPixmap input, const QSize& size ) const
{
    QPixmap output = input;
    QSize diff = input.size() - size;
    
    if( diff.height() > 0 || diff.width() > 0 )
    {
        const QPixmap scaled = input.scaled( size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation );
        const QPixmap cropped = scaled.copy( ((scaled.width() - size.width()) / 2), ((scaled.height() - size.height()) / 2), size.width(), size.height());
        output = cropped;
    }
    return output;
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

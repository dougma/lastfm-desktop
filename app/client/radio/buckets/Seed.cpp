/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
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

#include "Seed.h"
#include "SeedListModel.h"
#include "SeedListView.h"
#include <lastfm/WsReply>
#include <QTimeLine>
#include <QPainter>
#include "app/moose.h"
#include "PlayableMimeData.h"


Seed::Seed( SeedListView* parent ) 
     :QObject( parent )
{
    setupItem();
    m_networkManager = new WsAccessManager( this );
}


Seed::Seed( const QString& name, SeedListView* parent )
     :QObject( parent ), m_name( name )
{
    setupItem();
    m_networkManager = new WsAccessManager( this );
    parent->seedModel()->addItem( this ); 
}


Seed* /* static */
Seed::createFromMimeData( const PlayableMimeData* data, SeedListView* parent )
{
    Seed* item = new Seed( parent );
    
    item->setName( data->text() );
    
    item->setPlayableType( data->type() );
    
    if( data->hasImage() )
        item->setIcon( QIcon( QPixmap::fromImage( data->imageData().value<QImage>())) );
    else
        item->fetchImage();
    
    return item;
};


void 
Seed::iconDataDownloaded()
{
    QNetworkReply* reply = static_cast< QNetworkReply* >( sender());

    QPixmap pixmap;
    pixmap.loadFromData( reply->readAll() );

    if( pixmap.isNull() )
        return;

    setPixmap( pixmap );
}


void
Seed::fetchImage()
{
    switch( playableType() )
    {
        case Seed::ArtistType:
        {
            Artist a( name() );
            WsReply* reply = a.search( 5 );

            connect( reply, SIGNAL(finished(WsReply*)), SLOT(onArtistSearchFinished(WsReply*)) );
            break;
        }

        case Seed::UserType:
        {
            //TODO user.search method - waiting on webteam
            break;
        }

        case Seed::TagType:
        {
            setIcon( QIcon( ":/buckets/tag_39.png" ) );
            break;
        }
            
        default:
            break;
    }
}


void 
Seed::onArtistSearchFinished( WsReply* r )
{
    try
    {
        QList<Artist> results = Artist::list( r );
        
        if( results.isEmpty() )
            return;
        
        Artist a = results.first();
        
        if (a.name().toLower() != name().toLower())
        {
            //TODO: handle exact artist not found case
            //      I'm going to work on the updated player bucket 
            //      before I dive into this.
            return;
        }
        
        if( a.imageUrl().isValid() )
        {
            QNetworkReply* get = m_networkManager->get( QNetworkRequest( a.imageUrl( lastfm::Small ) ));
            connect( get, SIGNAL( finished()), SLOT( iconDataDownloaded()));
        }
    }
    catch (std::runtime_error& e)
    {
        return;
    }
}


void 
Seed::setPixmap( const QPixmap pm )
{
    const QPixmap pm126 = cropToSize( pm, QSize( 126, 100 ));
    const QPixmap pm17 = cropToSize( pm, QSize( 17, 17 ));
    
    const QPixmap overlayedIcon126 = overlayPixmap( pm126, QPixmap( ":buckets/avatar_overlay_34.png" ), QPoint( 1, 1 ));
    const QPixmap overlayedIcon17 = overlayPixmap( pm17, QPixmap( ":buckets/avatar_overlay_17.png" ), QPoint( 1, 1 ));
    
    QPixmap selectedIcon126 = overlayedIcon126;
    {
        QRect selectRect = overlayedIcon126.rect().adjusted( 1, 1, -1, -3 );
        QPainter p( &selectedIcon126 );
        p.drawPixmap( selectRect, QPixmap( ":buckets/avatar_overlay_selected.png" ) );
    }

    QPixmap selectedIcon17 = overlayedIcon17;
    {
        QRect selectRect = overlayedIcon17.rect().adjusted( 1, 1, -1, -3 );
        QPainter p( &selectedIcon17 );
        p.drawPixmap( selectRect, QPixmap( ":buckets/avatar_overlay_selected.png" ) );
    }
    
    QIcon icon;
    icon.addPixmap( overlayedIcon126 );
    icon.addPixmap( selectedIcon126, QIcon::Selected );
    icon.addPixmap( overlayedIcon17 );
    icon.addPixmap( selectedIcon17, QIcon::Selected );
    setIcon( icon );
}


QPixmap 
Seed::overlayPixmap( const QPixmap source, const QPixmap overlay, const QPoint offset ) const
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
Seed::cropToSize( const QPixmap input, const QSize& size ) const
{
    QPixmap output = input;
    QSize diff = input.size() - size;
    
    if( abs(diff.height()) > 0 || abs(diff.width()) > 0 )
    {
        const QPixmap scaled = input.scaled( size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation );
        const QPixmap cropped = scaled.copy( ((scaled.width() - size.width()) / 2), 0, size.width(), size.height());
        output = cropped;
    }
    return output;
}

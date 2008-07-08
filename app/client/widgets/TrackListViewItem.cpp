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

#include "TrackListViewItem.h"
#include "lib/unicorn/ws/WsRequestManager.h"
#include "lib/unicorn/ws/WsReply.h"


TrackListViewItem::TrackListViewItem( const Track& t, QWidget* parent ) 
                 : QWidget( parent ),
                   m_track( t )
{
    ui.setupUi( this );
    ui.year->setEnabled( false );
    ui.year->setAttribute( Qt::WA_MacSmallSize );
    ui.album->setAttribute( Qt::WA_MacSmallSize );

    if (t.isEmpty())
    {
        //FIXME
        ui.artist->setText( "Super mega error :(" );
        return;
    }

    ui.artist->setText( t.artist() + ' ' + QChar(8211) + " <b>" + t.track() + "</b>" );
    ui.album->setText( t.album() );
    ui.year->setText( "2000" );
    
    
#if 0
    //TODO move into TrackInfo
    WsReply* reply = WsRequestBuilder( "track.getTopTags" )
                .add( "track", t.track() )
                .add( "artist", t.artist() )
                .get()
                .synchronously();
    
    QDomNodeList nodes = reply->domDocument().documentElement().elementsByTagName( "tag" );

    QStringList tags;
    for (int x = 0; x < nodes.count(); ++x)
    {
        tags += nodes.at( x ).firstChildElement( "name" ).text();
    }
    
    ui.tags->setText( tags.join( ", " ) );
#endif
}

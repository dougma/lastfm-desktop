/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd                                       *
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

#include "TrackListView.h"
#include "TrackListViewItem.h"
#include "lib/moose/TrackInfo.h"


TrackListView::TrackListView()
{
    new QVBoxLayout( this );
}


void 
TrackListView::add( const TrackInfo& t )
{
    TrackListViewItem* i = new TrackListViewItem;
    i->ui.artist->setText( t.artist() + " <b>" + t.track() + "</b>" );
    i->show();

    static_cast<QBoxLayout*>(layout())->insertWidget( 0, i );
}

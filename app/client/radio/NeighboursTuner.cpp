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

#include "NeighboursTuner.h"
#include "StationDelegate.h"
#include "lib/types/User.h"
#include "Settings.h"
#include "lib/radio/RadioStation.h"


NeighboursTuner::NeighboursTuner()
{
	setItemDelegate( new StationDelegate );
	User u( The::settings().username() );
	WsReply* reply = u.getNeighbours();
	connect( reply, SIGNAL( finished( WsReply*)), SLOT(onFetchedNeighbours( WsReply*)) );
	connect( this, SIGNAL( itemClicked( QListWidgetItem* )), SLOT( onUserClicked( QListWidgetItem*)) );
}


void
NeighboursTuner::onFetchedNeighbours( WsReply* r )
{
	const WeightedStringList& neighbours = User::getNeighbours( r );
	
	if( neighbours.isEmpty() ) 
		return;
	
	static_cast<StationDelegate*>( itemDelegate() )->setMaxCount( neighbours.first().weighting() );
	
	foreach( const WeightedString& neighbour, neighbours )
	{
		QListWidgetItem* i = new QListWidgetItem( neighbour, this );
		i->setData( StationDelegate::CountRole, neighbour.weighting() );
		addItem( i );
	}
}


void
NeighboursTuner::onUserClicked( QListWidgetItem* i )
{
	const QString& neighbour = i->data( Qt::DisplayRole ).toString();
	RadioStation r( neighbour, RadioStation::Library );
	emit tune( r );
	i->setSelected( false );
}
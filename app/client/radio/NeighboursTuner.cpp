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

#include "NeighboursTuner.h"
#include "StationDelegate.h"
#include "lib/lastfm/radio/RadioStation.h"
#include "lib/lastfm/types/User.h"
#include "lib/lastfm/ws/WsAccessManager.h"
#include "lib/lastfm/ws/WsReply.h"

Q_DECLARE_METATYPE(QListWidgetItem*);


NeighboursTuner::NeighboursTuner()
{
	m_networkManager = new WsAccessManager( this );
	setItemDelegate( new StationDelegate );
	WsReply* reply = AuthenticatedUser().getNeighbours();
	connect( reply, SIGNAL( finished( WsReply*)), SLOT(onFetchedNeighbours( WsReply*)) );
	connect( this, SIGNAL( itemClicked( QListWidgetItem* )), SLOT( onUserClicked( QListWidgetItem*)) );
}


void
NeighboursTuner::onFetchedNeighbours( WsReply* r )
{
	QList<User> neighbours = User::list( r );
	
	if( neighbours.isEmpty() ) 
		return;
	
	static_cast<StationDelegate*>( itemDelegate() )->setMaxCount( neighbours.first().match() );
	
	foreach( const User& user, neighbours )
	{
		QListWidgetItem* i = new QListWidgetItem( user, this );
		i->setData( Qt::DecorationRole, QImage( ":/lastfm/no/avatar.png" ));
		i->setData( StationDelegate::CountRole, user.match() );
		addItem( i );
		
		if( user.smallImageUrl().isEmpty() )
			continue;
		
		QNetworkRequest request( user.smallImageUrl() );
		
		request.setAttribute( QNetworkRequest::User, QVariant::fromValue<QListWidgetItem*>( i ));
		QNetworkReply* get = m_networkManager->get( request );
		connect( get, SIGNAL(finished()), SLOT( onImageDownloaded()) );
		
	}
}


void
NeighboursTuner::onUserClicked( QListWidgetItem* i )
{
	QString neighbour = i->data( Qt::DisplayRole ).toString();
	emit tune( RadioStation::library( neighbour ) );
	i->setSelected( false );
}


void
NeighboursTuner::onImageDownloaded()
{
	QNetworkReply* r = static_cast<QNetworkReply*>(sender());
	QListWidgetItem* i = r->request().attribute( QNetworkRequest::User ).value< QListWidgetItem*>();

	if( r->error() != QNetworkReply::NoError )
		return;
	
	i->setData( Qt::DecorationRole, QImage::fromData( r->readAll() ));
}
/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "FriendsTuner.h"
#include <lastfm/User.h"
#include "StationDelegate>
#include <lastfm/RadioStation>
#include <lastfm/WsAccessManager>
#include <lastfm/WsReply>
#include <QNetworkRequest>


Q_DECLARE_METATYPE( QListWidgetItem* );


FriendsTuner::FriendsTuner()
{
	m_networkManager = new WsAccessManager( this );
	setItemDelegate( new StationDelegate );
	setSortingEnabled( true );
	WsReply* reply = AuthenticatedUser().getFriends();
	connect( reply, SIGNAL( finished( WsReply*)), SLOT(onFetchedFriends( WsReply*)) );
	connect( this, SIGNAL( itemClicked( QListWidgetItem* )), SLOT( onFriendClicked( QListWidgetItem*)) );
}


void
FriendsTuner::onFetchedFriends( WsReply* r )
{	
	foreach( const User& user, User::list( r ) )
	{
		QListWidgetItem* item = new QListWidgetItem( user );
		item->setData( Qt::DecorationRole, QImage( ":/lastfm/no/avatar.png" ));
		addItem( item );
		
		if( user.smallImageUrl().isEmpty() )
			continue;
		
		QNetworkRequest request( user.smallImageUrl() );
		
		request.setAttribute( QNetworkRequest::User, QVariant::fromValue<QListWidgetItem*>( item ));
		QNetworkReply* get = m_networkManager->get( request );
		connect( get, SIGNAL(finished()), SLOT( onImageDownloaded()) );

	}
}


void
FriendsTuner::onFriendClicked( QListWidgetItem* i )
{
	User user = i->data( Qt::DisplayRole ).toString();
	emit tune( RadioStation::library( user ) );
	i->setSelected( false );
}


void
FriendsTuner::onImageDownloaded()
{
	QNetworkReply* r = static_cast<QNetworkReply*>(sender());
	QListWidgetItem* i = r->request().attribute( QNetworkRequest::User ).value< QListWidgetItem*>();

	if( r->error() != QNetworkReply::NoError )
		return;
	
	i->setData( Qt::DecorationRole, QImage::fromData( r->readAll() ));
}

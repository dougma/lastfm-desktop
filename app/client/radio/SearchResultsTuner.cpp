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

#include "SearchResultsTuner.h"
#include <QListWidget>
#include "StationDelegate.h"
#include "lib/lastfm/radio/RadioStation.h"
#include "the/app.h"
#include <QListWidgetItem>

SearchResultsTuner::SearchResultsTuner()
{
	addTab( ui.artistList = new QListWidget, tr( "Artists" ) );
	addTab( ui.tagList = new QListWidget, tr( "Tags" ) );
	
	ui.artistList->setItemDelegate( new StationDelegate );
	ui.tagList->setItemDelegate( new StationDelegate );
	
	connect( ui.artistList, SIGNAL( itemClicked( QListWidgetItem* )), SLOT( onArtistClicked( QListWidgetItem* )) );
	connect( ui.tagList, SIGNAL( itemClicked( QListWidgetItem* )), SLOT( onTagClicked( QListWidgetItem* )) );
	
}


void
SearchResultsTuner::addArtists( QList<Artist> al )
{
    foreach( const Artist& a, al )
        ui.artistList->addItem( a );
}


void
SearchResultsTuner::addTags( QStringList t )
{
	ui.tagList->addItems( t );
}


void 
SearchResultsTuner::onArtistClicked( QListWidgetItem* i )
{
	i->setSelected( false );
	Artist artist = i->data( Qt::DisplayRole ).toString();
	The::app().open( RadioStation::similar( artist ) );
}


void 
SearchResultsTuner::onTagClicked( QListWidgetItem* i )
{
	Tag tag = i->data( Qt::DisplayRole ).toString();
	The::app().open( RadioStation::globalTag( tag ) );
	i->setSelected( false );
}
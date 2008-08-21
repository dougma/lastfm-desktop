/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *    This program is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include "NowPlayingTuner.h"
#include "App.h"
#include "StationDelegate.h"
#include "PlayerEvent.h"
#include "ObservedTrack.h"
#include "lib/radio/RadioController.h"
#include "lib/radio/RadioStation.h"
#include "lib/types/Track.h"
#include <QVBoxLayout>
#include <QToolBar>
#include <QLineEdit>


NowPlayingTuner::NowPlayingTuner()
{
	ui.setupUi( this );
	
	ui.tagsTab->setItemDelegate( new StationDelegate( this ) );
	ui.similarArtistsTab->setItemDelegate( new StationDelegate( this ) );

	QLineEdit* tuning_dial = new QLineEdit;
    connect( tuning_dial, SIGNAL(returnPressed()), SLOT(onTunerReturnPressed()) );
	
	connect( qApp, SIGNAL(event( int, const QVariant& )), SLOT(onAppEvent( int, const QVariant& )) );
	
	connect( ui.tagsTab, SIGNAL(itemClicked( QListWidgetItem*)), SLOT(onTagClicked( QListWidgetItem*)) );
	connect( ui.similarArtistsTab, SIGNAL(itemClicked( QListWidgetItem*)), SLOT(onArtistClicked( QListWidgetItem*)) );
	
	QWidget* tempPage = new QWidget();
	QVBoxLayout* l = new QVBoxLayout;
	tempPage->setLayout( l );

    l->addWidget( tuning_dial );
	ui.tabWidget->addTab( tempPage, "Temp" );
	
}


void 
NowPlayingTuner::onTunerReturnPressed()
{
	QString url = static_cast<QLineEdit*>(sender())->text();
	emit tune( RadioStation( url, RadioStation::SimilarArtist ));
}


void
NowPlayingTuner::onAppEvent( int e, const QVariant& d )
{
	switch( e )
	{
		case PlayerEvent::PlaybackStarted:
		case PlayerEvent::TrackChanged:
		{
			Track t = d.value<ObservedTrack>();
			
			//Load Tags
			ui.tagsTab->clear();
			WsReply* r = t.getTopTags();
			connect( r, SIGNAL( finished( WsReply*)), SLOT(onFetchedTopTags(WsReply*)) );
			
			//Load Similar Artists
			ui.similarArtistsTab->clear();
			WsReply* similarReply = t.artist().getSimilar();
			connect( similarReply, SIGNAL( finished( WsReply*)), SLOT( onFetchedSimilarArtists(WsReply*)) );
		}
		break;

		case PlayerEvent::PlaybackEnded:
			ui.tagsTab->clear();
			ui.similarArtistsTab->clear();
		break;
	}
}


void addWeightedStringsToList( WeightedStringList& stringList, QListWidget* list )
{
	if( stringList.isEmpty() )
		return;

	stringList.sortWeightingDescending();
	
	float maxTagCount = stringList.first().weighting();

	StationDelegate* delegate = static_cast<StationDelegate*>( list->itemDelegate() );
	delegate->setMaxCount( maxTagCount );

	foreach( const WeightedString& weightedString, stringList )
	{
		QListWidgetItem* item = new QListWidgetItem( weightedString );
		item->setData( StationDelegate::CountRole, weightedString.weighting() );
		list->addItem( item );
	}
}


void
NowPlayingTuner::onFetchedTopTags( WsReply* r )
{
	WeightedStringList tags = Track::getTopTags( r );
	addWeightedStringsToList( tags, ui.tagsTab );
}


void
NowPlayingTuner::onFetchedSimilarArtists( WsReply* r )
{
	qDebug() << r->lfm();
	WeightedStringList artists = Artist::getSimilar( r );
	addWeightedStringsToList( artists, ui.similarArtistsTab );
}


void
NowPlayingTuner::onTagClicked( QListWidgetItem* i )
{
	RadioStation r( i->data( Qt::DisplayRole ).toString(), RadioStation::Tag );
	The::app().radioController().play( r );
}


void
NowPlayingTuner::onArtistClicked( QListWidgetItem* i )
{
	RadioStation r( i->data( Qt::DisplayRole ).toString(), RadioStation::SimilarArtist );
	The::app().radioController().play( r );
}
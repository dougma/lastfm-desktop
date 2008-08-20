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
#include "lib/radio/RadioStation.h"
#include "StationDelegate.h"
#include "App.h"
#include "ObservedTrack.h"
#include "PlayerEvent.h"
#include "lib/types/Track.h"
#include <QVBoxLayout>
#include <QToolBar>
#include <QLineEdit>


NowPlayingTuner::NowPlayingTuner()
{
	ui.setupUi( this );
	
	ui.tagsTab->setItemDelegate( new StationDelegate( this ) );

	QLineEdit* tuning_dial = new QLineEdit;
    connect( tuning_dial, SIGNAL(returnPressed()), SLOT(onTunerReturnPressed()) );
	connect( &The::app(), SIGNAL(event( int, const QVariant& )), SLOT(onAppEvent( int, const QVariant& )) );

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
		case PlayerEvent::TrackChanged:
		{
			Track t = d.value<ObservedTrack>();
			ui.tagsTab->clear();
			WsReply* r = t.getTopTags();
			connect( r, SIGNAL( finished( WsReply*)), SLOT(onFetchedTopTags(WsReply*)) );
		}
		break;

		case PlayerEvent::PlaybackEnded:
			ui.tagsTab->clear();
		break;
	}
}


void
NowPlayingTuner::onFetchedTopTags(WsReply* r)
{
	ui.tagsTab->addItems( Track::getTopTags( r ));
}
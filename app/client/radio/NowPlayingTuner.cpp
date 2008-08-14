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
#include <QVBoxLayout>
#include <QToolBar>
#include <QLineEdit>


NowPlayingTuner::NowPlayingTuner()
{
	ui.setupUi( this );
	ui.tagsTab->addItem( new QListWidgetItem( QIcon( ":/station.png" ), "Test" ));

	QLineEdit* tuning_dial = new QLineEdit;
    connect( tuning_dial, SIGNAL(returnPressed()), SLOT(onTunerReturnPressed()) );

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
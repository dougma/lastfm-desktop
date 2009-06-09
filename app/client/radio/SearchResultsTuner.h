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
#ifndef SEARCH_RESULTS_TUNER_H
#define SEARCH_RESULTS_TUNER_H

#include <lastfm/global.h>
#include <QTabWidget>


class SearchResultsTuner : public QTabWidget
{
	Q_OBJECT

public:
	SearchResultsTuner();
	void addArtists( QList<Artist> );
	void addTags( QStringList );

private slots:
	void onArtistClicked( class QListWidgetItem* );
	void onTagClicked( class QListWidgetItem* );

private:
	struct
	{
		class QListWidget* artistList;
		class QListWidget* tagList;
	} ui;
	
};

#endif
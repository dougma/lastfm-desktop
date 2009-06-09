/*
   Copyright 2009 Last.fm Ltd. 
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
#ifndef PLAYLIST_DIALOG_H
#define PLAYLIST_DIALOG_H

#include <lastfm/Track>
#include <QDialog>


class PlaylistDialog : public QDialog
{
	Q_OBJECT
	
	Track m_track;
    class UserPlaylistsModel* m_model;
    
    void setupUi();

    virtual void accept();
    
public:
	PlaylistDialog( const Track&, QWidget* parent );

    Track track() const { return m_track; }
	
	struct Ui
	{
		class PlaylistsView* playlists;
		class QDialogButtonBox* buttons;
        class TrackWidget* track;
	}
	ui;
	
public slots:
    void create();
    
private slots:
	void onGotUserPlaylists( WsReply* );
    void onPlaylistCreated( WsReply* );
};

#endif //PLAYLIST_DIALOG_H
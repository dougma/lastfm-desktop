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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

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
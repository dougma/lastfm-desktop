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

#ifndef BATCH_SCROBBLE_DIALOG_H
#define BATCH_SCROBBLE_DIALOG_H

#include <lastfm/Track.h"
#include "ui_BatchScrobbleDialog>


class BatchScrobbleDialog : public QDialog
{
    Q_OBJECT

public:
    BatchScrobbleDialog( QWidget* parent );
    
    void setTracks( const QList<Track>& );
    QList<Track> tracks() const;

public slots:
    void toggleChecked();

private:
    void setupUi();
    void addTracksToView();

    virtual void showEvent( QShowEvent* );

    Ui::MediaDeviceConfirmDialog ui;

    QString const m_username;
    QList<Track> m_tracks;
};

#endif

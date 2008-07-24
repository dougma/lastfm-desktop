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

#ifndef SHARE_DIALOG_H
#define SHARE_DIALOG_H

#include "lib/types/Track.h"
#include "ui_ShareDialog.h"


class ShareDialog : public QDialog
{
    Q_OBJECT

public:
    ShareDialog( QWidget* parent );

    /** for the love of all that is holy, call this before show! */
    void setTrack( const Track& );
	Track track() const { return m_track; }

    Ui::ShareDialog ui;

private slots:
    void onFriendsReturn( class WsReply* );
    void enableDisableOk();

private:
    class QPushButton* ok() { return ui.buttons->button( QDialogButtonBox::Ok ); }

    virtual bool eventFilter( QObject*, QEvent* );
    virtual void accept();

    Track m_track;
};

#endif

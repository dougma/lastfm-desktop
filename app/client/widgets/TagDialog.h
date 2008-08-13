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

#ifndef TAG_DIALOG_H
#define TAG_DIALOG_H

#include "lib/types/Track.h"
#include "ui_tagdialog.h"


class TagDialog : public QDialog
{
    Q_OBJECT

public:
    TagDialog( QWidget *parent );

	Track track() const { return m_track; }
	
	/** you can only call this once, and you must call it before you show or exec() */
	void setTrack( const Track& t );
	
    void setTaggingType( int i ) { ui.tagTypeBox->setCurrentIndex( i ); }

private slots:
    void searchAsYouType( const QString& );

    void onAccepted();
    void onTagTypeChanged( int type );
    void onWsFinished( WsReply* );
    void onTagActivated( QTreeWidgetItem *item );
    void onTagEditChanged();

    void saveSettings();

private:
    void follow( WsReply* );
    virtual bool eventFilter( QObject*, QEvent* );

private:
    Ui::TagDialog ui;

    Track m_track;
    QStringList m_originalTags;
    QStringList m_publicTags;
    QStringList m_userTags;

    QList<WsReply*> m_activeRequests;
};

#endif

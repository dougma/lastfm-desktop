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

#include "lib/lastfm/types/Track.h"
#include <QModelIndex>
#include <QDialog>

namespace Unicorn 
{
    class TabWidget;
}


class TagDialog : public QDialog
{
    Q_OBJECT

public:
    TagDialog( const Track&,QWidget *parent );

	Track track() const { return m_track; }

private slots:
    void onWsFinished( WsReply* );
    void onTagActivated( class QTreeWidgetItem *item );
    void onAddClicked();
    
    void removeCurrentTag();

private:
    struct Ui
    {
        class TrackWidget* track;
        class SpinnerLabel* spinner;
        class QLineEdit* edit;
        class QPushButton* add;
        class TagBuckets* appliedTags;
        class TagListWidget* suggestedTags;
        class TagListWidget* yourTags;
        class QDialogButtonBox* buttons;
        Unicorn::TabWidget* tabs;
        
        void setupUi( QWidget* parent );
    } ui;

    void setupUi();
    void follow( WsReply* );
    
    virtual void accept();
    
    Track m_track;
    QStringList m_originalTags;
    QStringList m_publicTags;
    QStringList m_userTags;

    QList<WsReply*> m_activeRequests;
};

#endif

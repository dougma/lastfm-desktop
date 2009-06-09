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
#ifndef TAG_DIALOG_H
#define TAG_DIALOG_H

#include <lastfm/Track>
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
    TagDialog( const Track&, QWidget* parent );

	Track track() const { return m_track; }

private slots:
    void onWsFinished( QNetworkReply* );
    void onTagActivated( class QTreeWidgetItem *item );
    void onAddClicked();
    void onTagListItemDoubleClicked( QTreeWidgetItem*, int);
    void follow( QNetworkReply* );
    void removeCurrentTag();

private:
    struct Ui
    {
        class TrackWidget* track;
        class SpinnerLabel* spinner;
        class TagBuckets* appliedTags;
        class TagListWidget* suggestedTags;
        class TagListWidget* yourTags;
        class QDialogButtonBox* buttons;
        Unicorn::TabWidget* tabs;
        
        void setupUi( QWidget* parent );
    } ui;

    void setupUi();
    
    virtual void accept();
    
    Track m_track;
    QStringList m_originalTags;
    QStringList m_publicTags;
    QStringList m_userTags;

    QList<QNetworkReply*> m_activeRequests;
};

#endif

/*
   Copyright 2005-2009 Last.fm Ltd. 

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

#ifndef SOURCE_ITEM_WIDGET_H
#define SOURCE_ITEM_WIDGET_H

#include "lib/unicorn/StylableWidget.h"
#include <QModelIndex>

class QAbstractItemModel;
class QNetworkReply;

class SourceItemWidget : public StylableWidget
{
    Q_OBJECT

public:
    SourceItemWidget();
    SourceItemWidget(const QString& labelText);
    void getImage(const QUrl& url);

public slots:
    void onGotImage();

signals:
    void deleteClicked();

protected:
    class QLabel* m_label;
    class QLabel* m_image;
    QString m_rql;
};


class UserItemWidget : public SourceItemWidget
{
    Q_OBJECT

public:
    UserItemWidget(const QString& username);
    void setModel(QAbstractItemModel* model, const QModelIndex& index);

protected:
    QString m_username;
    class QComboBox* m_combo;
    class QComboBox* m_playlistCombo;
    class QComboBox* m_tagCombo;

private slots:
    void onComboChanged(int newIdx);
    void onCombo2Changed(int newIdx);
    void onGotPlaylists();
    void onGotTags();

private:
    QAbstractItemModel* m_model;
    QModelIndex m_index;
};

#endif

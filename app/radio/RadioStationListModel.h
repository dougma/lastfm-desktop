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

#ifndef RADIO_STATION_LIST_MODEL_H
#define RADIO_STATION_LIST_MODEL_H

#include <QAbstractListModel>
#include <QModelIndex>
#include <lastfm/RadioStation>

class RadioStationListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum RadioStationListModelRole { UrlRole = Qt::UserRole };

    RadioStationListModel(QObject* parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    void setList(QList<RadioStation> list);

private:
    QList<RadioStation> m_list;
};

#endif

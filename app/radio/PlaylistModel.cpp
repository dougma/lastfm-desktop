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

#include "PlaylistModel.h"

PlaylistModel::PlaylistModel(QObject* parent)
: QAbstractListModel(parent)
{
}

int
PlaylistModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_list.count();
}

QVariant
PlaylistModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_list.size())
        return QVariant();

    PlaylistMeta i = m_list.at(index.row());

    switch (role) {
        case Qt::DisplayRole: return i.title;
        case Qt::ToolTipRole: return i.description;
        case PlaylistIdRole: return i.id;
    }

    return QVariant();    

}

bool
PlaylistModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return true;
}

void
PlaylistModel::setList(QList<PlaylistMeta> list)
{
    m_list = list;
}
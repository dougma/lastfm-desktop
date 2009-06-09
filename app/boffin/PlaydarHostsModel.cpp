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
#include "PlaydarHostsModel.h"


//virtual 
QVariant 
PlaydarHostsModel::data(const QModelIndex &index, int role /* = Qt::DisplayRole */) const
{
    return (index.row() < m_hosts.size()) ? m_hosts[index.row()] : QVariant();
}

//virtual 
int 
PlaydarHostsModel::rowCount(const QModelIndex &parent /* = QModelIndex() */) const
{
    return m_hosts.size();
}

void 
PlaydarHostsModel::onHosts(const QStringList& hosts)
{
    m_hosts = hosts;
    reset();
}

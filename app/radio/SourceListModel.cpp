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

#include "SourceListModel.h"
#include <QStringList>
#include <stdexcept>


SourceListModel::SourceListModel(int maxSize, QObject* parent)
    : QAbstractListModel(parent)
    , m_maxSize(maxSize)
{
}

// returns the index of the added entry
// -1 if not added (because maxSize has been reached)
int
SourceListModel::addSource(const RqlSource& s)
{
    int c = m_list.size();
    if (c < m_maxSize) {
        beginInsertRows(QModelIndex(), c, c);
        m_list << s;
        endInsertRows();
        return c;
    }
    return -1;
}

void
SourceListModel::removeSource(int idx)
{
    if (idx >= 0 && idx < m_list.size()) {
        beginRemoveRows(QModelIndex(), idx, idx);
        m_list.removeAt(idx);
        endRemoveRows();
    }
}

int
SourceListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_list.count();
}

QVariant
SourceListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_list.size())
        return QVariant();

    RqlSource src = m_list.at(index.row());

    switch (role) {
        case Qt::DisplayRole: return src.toDisplayString();
        case SourceType: return src.type;
        case Arg1: return src.arg1;
        case Arg2: return src.arg2;
        case Weight: return src.weight;
        case Rql: return src.toRqlString();
        case ImageUrl: return src.imgUrl;
    }

    return QVariant();    
}

int
SourceListModel::getMaxSize() const
{
    return m_maxSize;
}

QStringList
SourceListModel::rql() const
{
    QStringList result;
    foreach (const RqlSource& r, m_list) {
        result << r.toRqlString();
    }
    return result;
}

QStringList
SourceListModel::descriptions() const
{
    QStringList result;
    foreach (const RqlSource& r, m_list) {
        result << r.toDisplayString();
    }
    return result;
}

bool
SourceListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && index.row() < m_list.size()) {
        switch (role) {
            case SourceListModel::SourceType:
                m_list[index.row()].type = (RqlSource::Type) value.toInt();
                return true;
            case SourceListModel::Arg1:
                m_list[index.row()].arg1 = value.toString();
                return true;
            case SourceListModel::Arg2:
                m_list[index.row()].arg2 = value.toString();
                return true;
        }
    }
    return false;
}

////

RqlSource::RqlSource(Type _type, const QString& _arg1, const QString& _arg2, float _weight, const QString& _imgUrl)
: type(_type)
, arg1(_arg1)
, arg2(_arg2)
, weight(_weight)
, imgUrl(_imgUrl)
{
}

QString
RqlSource::sourceName() const
{
    switch (type) {
        case SimArt: return "simart";
        case Tag: return "tag";
        case User: return "user";
        case Group: return "group";
        case Rec: return "rec";
        case Loved: return "loved";
        case Neigh: return "neigh";
        case PersonalTag: return "ptag";
        case Playlist: return "playlist";
        case Art: return "art";
    } 
    throw std::runtime_error("unknown type in RqlSource");
}

QString
RqlSource::toDisplayString() const
{
    switch (type) {
        case SimArt: return arg1;
        case Tag: return arg1 + " Tag";
        case User: return arg1 + "'s Library";
        case Group: return arg1 + " Group";
        case Rec: return arg1 + "'s Recommendations";
        case Loved: return arg1 + "'s Loved Tracks";
        case Neigh: return arg1 + "'s Neighbourhood";
        case PersonalTag: return arg2 + "'s " + arg1 + " Tag";
        case Playlist: return "Playlist " + arg1;  // todo: make this one more descriptive?
        case Art: return arg1 + " Artist";
    } 
    throw std::runtime_error("unknown type in RqlSource");
}

QString
RqlSource::toRqlString() const
{
    QString r = sourceName() + ":" + quote(arg1);
    if (arg2.length()) {
        r += "|" + quote(arg2);
    }
    if (weight != 1.0) {
        r += "^" + QString::number(weight, 'f');
    }
    return r;
}
        
//static 
QString
RqlSource::quote(QString s)
{
    s.remove('\"');    // quotes are unescapable :/
    if (s.contains('[') || s.contains(' ') || s.contains(']')) {
        return "\"" + s + "\"";
    }
    return s;
}

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

#ifndef SOURCE_LIST_MODEL_H
#define SOURCE_LIST_MODEL_H

#include <QAbstractListModel>
#include <QList>

struct RqlSource
{
    enum Type
    {
        SimArt, Tag, User, Group, Rec, Loved, Neigh, PersonalTag, Playlist, Art
    };

    RqlSource(Type type, const QString& arg1, const QString& arg2, float weight, const QString& imgUrl = QString());
    
    // rql parameters:
    Type type;
    QString arg1;
    QString arg2;
    float weight;

    // 
    QString imgUrl;

    //////////////

    QString sourceName() const;
    QString toDisplayString() const;
    QString toRqlString() const;
    static QString quote(QString);
};


class SourceListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    SourceListModel(int maxSize, QObject* parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    int getMaxSize() const;
    int addSource(const RqlSource& s);
    void removeSource(int idx);
    QStringList rql() const;
    QStringList descriptions() const;

    enum Role
    {
        SourceType = Qt::UserRole,
        Arg1,
        Arg2,
        Weight,
        Rql,
        ImageUrl
    };

private:

    int m_maxSize;
    QList<RqlSource> m_list;
};

#endif

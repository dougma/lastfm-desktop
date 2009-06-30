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

#ifndef SEARCH_BOX_H
#define SEARCH_BOX_H

#include <QLineEdit>
#include <lastfm/global.h>

class QNetworkReply;
class QCompleter;

namespace lastfm {
    class XmlQuery;
}


class SearchBox : public QLineEdit
{
    Q_OBJECT;

public:
    SearchBox(QWidget *parent = 0);

signals:
    void selected(const QString& item);

private slots:
    void onTextEdited(const QString& text);
    void onSearchFinished();
    void onCompleterActivated(const QString& text);

protected:
    virtual QNetworkReply* startSearch(const QString& term) = 0;
    virtual QStringList handleSearchResponse(XmlQuery& lfm) = 0;

    QCompleter* m_completer;
    bool m_searching;
};


class ArtistSearch : public SearchBox
{
public:
    ArtistSearch(QWidget *parent = 0);
    virtual QNetworkReply* startSearch(const QString& term);
    virtual QStringList handleSearchResponse(XmlQuery& lfm);
};


class TagSearch : public SearchBox
{
public:
    TagSearch(QWidget *parent = 0);
    virtual QNetworkReply* startSearch(const QString& term);
    virtual QStringList handleSearchResponse(XmlQuery& lfm);

};


class UserSearch : public SearchBox
{
public:
    UserSearch(QWidget *parent = 0);
    virtual QNetworkReply* startSearch(const QString& term);
    virtual QStringList handleSearchResponse(XmlQuery& lfm);
};

#endif


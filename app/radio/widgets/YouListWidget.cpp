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

#include "YouListWidget.h"
#include "..\SourceListModel.h"
#include <lastfm.h>


YouListWidget::YouListWidget(const QString& username, QWidget* parent)
    : QTreeWidget(parent) 
    , m_username(username)
{
    QString displayName = (username == lastfm::ws::Username) ? "Your " : username + "'s ";

    setAlternatingRowColors( true );
    setHeaderHidden( true );
    setIndentation( 10 );
    //setRootIsDecorated( true );
    //setExpandsOnDoubleClick( false );
    //setItemsExpandable( true );
    QList<QTreeWidgetItem*> headerItems;
    QTreeWidgetItem *h, *item;
    headerItems << (h = new QTreeWidgetItem(QStringList(displayName + "Stations")));

    item = new QTreeWidgetItem(h, QStringList(displayName + "Library"));
    item->setData(0, SourceListModel::SourceType, RqlSource::User);
    item->setData(0, SourceListModel::Arg1, username);
    item = new QTreeWidgetItem(h, QStringList(displayName + "Loved Tracks"));
    item->setData(0, SourceListModel::SourceType, RqlSource::Loved);
    item->setData(0, SourceListModel::Arg1, username);
    item = new QTreeWidgetItem(h, QStringList(displayName + "Recommendations"));
    item->setData(0, SourceListModel::SourceType, RqlSource::Rec);
    item->setData(0, SourceListModel::Arg1, username);
    item = new QTreeWidgetItem(h, QStringList(displayName + "Neighbourhood"));
    item->setData(0, SourceListModel::SourceType, RqlSource::Neigh);
    item->setData(0, SourceListModel::Arg1, username);

    headerItems << (m_friendsItem = new QTreeWidgetItem(QStringList(displayName + "Friends")));
    headerItems << (m_artistsItem = new QTreeWidgetItem(QStringList(displayName + "Artists")));
    headerItems << (m_tagsItem = new QTreeWidgetItem(QStringList(displayName + "Tags")));
    headerItems << (m_playlistsItem = new QTreeWidgetItem(QStringList(displayName + "Playlists")));

    lastfm::User user(username);
    connect(user.getFriends(), SIGNAL(finished()), SLOT(gotFriends()));
    connect(user.getTopArtists(), SIGNAL(finished()), SLOT(gotTopArtists()));
    connect(user.getTopTags(), SIGNAL(finished()), SLOT(gotTopTags()));
    connect(user.getPlaylists(), SIGNAL(finished()), SLOT(gotPlaylists()));

    addTopLevelItems( headerItems );
    h->setExpanded( true );
    h->setFlags( h->flags() & ~Qt::ItemIsSelectable );
}

void
YouListWidget::gotFriends()
{
    sender()->deleteLater();
    QNetworkReply* r = (QNetworkReply*)sender();
    lastfm::XmlQuery lfm(r->readAll());

    foreach (lastfm::XmlQuery e, lfm["friends"].children("user")) {
        QString name = e["name"].text();
        QTreeWidgetItem* item = new QTreeWidgetItem(m_friendsItem, QStringList(name));
        item->setToolTip(0, e["realname"].text());
        item->setData(0, SourceListModel::SourceType, RqlSource::User);
        item->setData(0, SourceListModel::Arg1, name);
        item->setData(0, SourceListModel::ImageUrl, e["image size=small"].text());
    }
}

void
YouListWidget::gotTopArtists()
{
    sender()->deleteLater();
    QNetworkReply* r = (QNetworkReply*)sender();
    lastfm::XmlQuery lfm(r->readAll());

    foreach (lastfm::XmlQuery e, lfm["topartists"].children("artist")) {
        QString artist = e["name"].text();
        QTreeWidgetItem* item = new QTreeWidgetItem(m_artistsItem, QStringList(artist));
        item->setData(0, SourceListModel::SourceType, RqlSource::SimArt);
        item->setData(0, SourceListModel::Arg1, artist);
        item->setData(0, SourceListModel::ImageUrl, e["image size=small"].text());
    }
}

void
YouListWidget::gotTopTags()
{
    sender()->deleteLater();
    QNetworkReply* r = (QNetworkReply*)sender();
    lastfm::XmlQuery lfm(r->readAll());

    foreach (lastfm::XmlQuery e, lfm["toptags"].children("tag")) {
        QString tag = e["name"].text();
        QTreeWidgetItem* item = new QTreeWidgetItem(m_tagsItem, QStringList(tag));
        item->setData(0, SourceListModel::SourceType, RqlSource::PersonalTag);
        item->setData(0, SourceListModel::Arg1, tag);
        item->setData(0, SourceListModel::Arg2, m_username);
    }
}

void
YouListWidget::gotPlaylists()
{
    sender()->deleteLater();
    QNetworkReply* r = (QNetworkReply*)sender();
    lastfm::XmlQuery lfm(r->readAll());

    foreach (lastfm::XmlQuery e, lfm["playlists"].children("playlist")) {
        QTreeWidgetItem* item = new QTreeWidgetItem(m_playlistsItem, QStringList(e["title"].text()));
        item->setToolTip(0, e["description"].text());
        item->setData(0, SourceListModel::SourceType, RqlSource::Playlist);
        item->setData(0, SourceListModel::Arg1, e["id"].text().toInt());
        // todo: arg2 could carry the playlist title?
        item->setData(0, SourceListModel::ImageUrl, e["image size=small"].text());
    }
}

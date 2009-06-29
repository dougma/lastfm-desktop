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

#include <QTabWidget>
#include <QGridLayout>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <lastfm/User>
#include <lastfm/XmlQuery>
#include "MultiStarterWidget.h"
#include "SourceSelectorWidget.h"
#include "SourceListWidget.h"
#include "SearchBox.h"

#include <QGroupBox>


MultiStarterWidget::MultiStarterWidget(int maxSources, QWidget *parent)
    : QWidget(parent)
    , m_minTagCount(10)
    , m_minArtistCount(10)
{
    QGridLayout* grid = new QGridLayout(this);

    QTabWidget* tabwidget = new QTabWidget();

    m_artists = new SourceSelectorWidget(new ArtistSearch());    
    tabwidget->addTab(m_artists, tr("Artists"));
    connect(m_artists, SIGNAL(add(QString)), SLOT(onAdd(QString)));

    m_tags = new SourceSelectorWidget(new TagSearch());
    tabwidget->addTab(m_tags, tr("Tags"));
    connect(m_tags, SIGNAL(add(QString)), SLOT(onAdd(QString)));

    m_users = new SourceSelectorWidget(new TagSearch());
    tabwidget->addTab(m_users, tr("Friends"));
    connect(m_users, SIGNAL(add(QString)), SLOT(onAdd(QString)));

    m_sourceList = new SourceListWidget(maxSources);

    m_playButton = new QPushButton(tr("Play"));

    grid->addWidget(tabwidget, 0, 0, 2, 1);
    grid->addWidget(m_sourceList, 0, 1, 1, 1);
    grid->addWidget(m_playButton, 1, 1);
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 1);

    connect(m_playButton, SIGNAL(clicked()), SLOT(onPlayClicked()));

    ///
    lastfm::AuthenticatedUser you;
    connect(you.getTopTags(), SIGNAL(finished()), SLOT(onUserGotTopTags()));
    connect(you.getTopArtists(), SIGNAL(finished()), SLOT(onUserGotTopArtists()));
    connect(you.getFriends(), SIGNAL(finished()), SLOT(onUserGotFriends()));
}

void
MultiStarterWidget::onAdd(const QString& item)
{
    SourceListWidget::SourceType itemType;
    if (m_artists == sender()) {
        itemType = SourceListWidget::Artist;
    } else if (m_tags == sender()) {
        itemType = SourceListWidget::Tag;
    } else if (m_users == sender()) {
        itemType = SourceListWidget::User;
    } else {
        return;
    }

    if (m_sourceList->addSource(itemType, item)) {
        // todo: grey it out if it's in the list?  or grey it some other way?
    }
}


void
MultiStarterWidget::onUserGotTopTags()
{
    sender()->deleteLater();
    QNetworkReply* r = (QNetworkReply*)sender();
    lastfm::XmlQuery lfm(r->readAll());

    QStringList tags;
    foreach (lastfm::XmlQuery e, lfm["toptags"].children("tag")) {
        tags += e["name"].text();
    }
    m_tags->list()->insertItems(0, tags);
    if (tags.size() < m_minTagCount) {
        // get global top tags
    }
}

void
MultiStarterWidget::onUserGotTopArtists()
{
    sender()->deleteLater();
    QNetworkReply* r = (QNetworkReply*)sender();
    lastfm::XmlQuery lfm(r->readAll());

    QStringList artists;
    foreach (lastfm::XmlQuery e, lfm["topartists"].children("artist")) {
        artists += e["name"].text();
    }
    m_artists->list()->insertItems(0, artists);
    if (artists.size() < m_minArtistCount) {
        // get global top artists
    }
}

void
MultiStarterWidget::onUserGotFriends()
{
    sender()->deleteLater();
    QNetworkReply* r = (QNetworkReply*)sender();
    lastfm::XmlQuery lfm(r->readAll());

    QStringList friends;
    foreach (lastfm::XmlQuery e, lfm["friends"].children("user")) {
        friends += e["name"].text();
    }
    m_users->list()->insertItems(0, friends);
    if (friends.size() < m_minArtistCount) {
        // no friends. so?
    }
}

void
MultiStarterWidget::onPlayClicked()
{
    RadioStation r = RadioStation::rql(m_sourceList->rql());
    r.setTitle(m_sourceList->stationDescription());
    emit startRadio(r);
}

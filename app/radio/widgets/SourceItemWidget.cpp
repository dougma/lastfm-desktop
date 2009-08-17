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

#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QNetworkReply>
#include <QStringListModel>
#include <lastfm/ws.h>
#include <lastfm/Tag>
#include <lastfm/User>
#include <lastfm/XmlQuery>
#include "lib/unicorn/UnicornSettings.h"
#include "SourceItemWidget.h"
#include "../SourceListModel.h"


SourceItemWidget::SourceItemWidget()
:m_image(0)
,m_label(0)
{
}

SourceItemWidget::SourceItemWidget(const QString& labelText)
{
    QPushButton* del;
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget( m_image = new QLabel, 0, Qt::AlignLeft );
    layout->addWidget( m_label = new QLabel, 0, Qt::AlignLeft );
    layout->addWidget( del = new QPushButton("X"), 0, Qt::AlignRight );
    m_image->setObjectName( "image" );
    m_image->setProperty( "noImage", true );
    m_label->setText( labelText );
    connect(del, SIGNAL(clicked()), SIGNAL(deleteClicked()));
}

void
SourceItemWidget::getImage(const QUrl& url)
{
    QNetworkReply* reply = lastfm::nam()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), SLOT(onGotImage()));
}

void
SourceItemWidget::onGotImage()
{
    sender()->deleteLater();
    QNetworkReply* reply = (QNetworkReply*) sender();
    if (reply->error() == QNetworkReply::NoError) {
        QPixmap p;
        p.loadFromData(((QNetworkReply*)sender())->readAll());
        if (!p.isNull()) {
            // lose the default user image:
            m_image->setProperty( "noImage", false );
            // need to reapply stylesheet to pickup the property change :(  
            // (but this doesn't actually work (on at least qt 4.4.3 on windows))
            // fixme.
            m_image->setStyleSheet( m_image->styleSheet() );    

            // maximumSize() doesn't get the maximum size from the stylesheet :(
            // so we hard-code to 34x34. fixme.
            // QSize size = m_image->maximumSize();
            m_image->setPixmap(p.scaled(34, 34, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
}

////

UserItemWidget::UserItemWidget(const QString& username)
: m_username(username)
, m_personalTagsModel(0)
, m_playlistModel(0)
{
    // subscribers can listen to loved tracks and personal tags.
    bool subscriber = false;
    {
        unicorn::UserSettings us;
        QVariant v = us.value(unicorn::UserSettings::subscriptionKey(), false);
        subscriber = v.toBool();
    }

    // todo: no reason username can't come from the model...
    QPushButton* del;
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget( m_image = new QLabel );

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->addWidget( m_label = new QLabel );
    vlayout->addWidget( m_combo = new QComboBox() );
    vlayout->addWidget( m_combo2 = new QComboBox() );
    m_combo2->setVisible(false);
    m_combo->addItem( "Library", RqlSource::User );
    if (subscriber) {
        m_combo->addItem( "Loved Tracks", RqlSource::Loved );
    } else {
        // would be nice to show the option in a disabled state.
        // how?
        // fixme.
    }
    m_combo->addItem( "Recommended", RqlSource::Rec );
    m_combo->addItem( "Neighbours", RqlSource::Neigh );
    connect(m_combo, SIGNAL(currentIndexChanged(int)), SLOT(onComboChanged(int)));
    connect(m_combo2, SIGNAL(currentIndexChanged(int)), SLOT(onCombo2Changed(int)));

    layout->addLayout( vlayout );
    layout->addWidget( del = new QPushButton("X"), 0, Qt::AlignRight );
    m_image->setObjectName( "userImage" );
    m_image->setProperty( "noImage", true );
    m_label->setText( username );
    connect(del, SIGNAL(clicked()), SIGNAL(deleteClicked()));

    if (subscriber) {
        connect(User(username).getPlaylists(), SIGNAL(finished()), SLOT(onGotPlaylists()));
        connect(User(username).getTopTags(), SIGNAL(finished()), SLOT(onGotTags()));
    }
}

void
UserItemWidget::setModel(QAbstractItemModel* model, const QModelIndex& index)
{
    // todo: i'd like to do this right way, by setting the model on m_combo, 
    // but I can't make it work... hence the signal/slot workaround
    m_model = model;
    m_index = index;
}

void
UserItemWidget::onComboChanged(int comboItemIndex)
{
    QVariant v = m_combo->itemData(comboItemIndex);
    int i = v.toInt();
    m_model->setData(m_index, v, SourceListModel::SourceType);
    if (RqlSource::PersonalTag == i) {
        m_combo2->setModel(m_personalTagsModel);
        m_combo2->setVisible(true);
    } else if (RqlSource::Playlist == i) {
        m_combo2->setModel(m_playlistModel);
        m_combo2->setVisible(true);
    } else {
        m_combo2->setVisible(false);
    }
}

void
UserItemWidget::onCombo2Changed(int comboItemIndex)
{
    if (m_combo2->model() == m_personalTagsModel) {
        QVariant v = m_combo2->itemData(comboItemIndex, Qt::DisplayRole);       // tag name
        m_model->setData(m_index, v, SourceListModel::Arg1);
        m_model->setData(m_index, m_username, SourceListModel::Arg2);           
    } else if (m_combo2->model() == m_playlistModel) {
        QVariant v = m_combo2->itemData(comboItemIndex, Qt::UserRole);          // playlist id
        m_model->setData(m_index, v, SourceListModel::Arg1);
    } 
}

void
UserItemWidget::onGotPlaylists()
{
    QNetworkReply* r = qobject_cast<QNetworkReply*>(sender());
    if (r) {
        r->deleteLater();
        QList<lastfm::XmlQuery> playlists = lastfm::XmlQuery(r->readAll()).children("playlist");
        if (playlists.count()) {
            m_playlistModel = new QStringListModel(this);
            m_playlistModel->insertRows(0, playlists.count());
            int row = 0;
            foreach(const XmlQuery& playlist, playlists) {
                QModelIndex idx = m_playlistModel->index(row);
                m_playlistModel->setData(idx, playlist["title"].text(), Qt::DisplayRole);
                m_playlistModel->setData(idx, playlist["id"].text().toInt(), Qt::UserRole);
                row++;
            }
            m_combo->addItem( "Playlist", RqlSource::Playlist );
        }
    }
}

void
UserItemWidget::onGotTags()
{
    QNetworkReply* r = qobject_cast<QNetworkReply*>(sender());
    if (r) {
        r->deleteLater();
        QStringList tags = Tag::list(r).values();
        if (tags.count()) {
            m_personalTagsModel = new QStringListModel(this);
            m_personalTagsModel->setStringList(tags);
            m_combo->addItem( "Personal tag", RqlSource::PersonalTag );
        }
    }
}
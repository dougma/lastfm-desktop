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
#include "SourceItemWidget.h"


SourceItemWidget::SourceItemWidget()
:m_image(0)
,m_label(0)
{
}

SourceItemWidget::SourceItemWidget(const QString& labelText)
{
    QPushButton* del;
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget( m_image = new QLabel );
    layout->addWidget( m_label = new QLabel );
    layout->addWidget( del = new QPushButton("X") );
    m_image->setScaledContents( true );
    m_label->setText( labelText );
    connect(del, SIGNAL(clicked()), SIGNAL(deleteClicked()));
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
            m_image->setPixmap(p);
        }
    }
}

////

UserItemWidget::UserItemWidget(const QString& username)
: m_username(username)
{
    QPushButton* del;
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget( m_image = new QLabel );

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->addWidget( m_label = new QLabel );
    vlayout->addWidget( m_combo = new QComboBox() );
    m_combo->addItem( "Library", "user" );
    m_combo->addItem( "Loved Tracks", "loved" );
    m_combo->addItem( "Recommended", "recs" );
    m_combo->addItem( "Neighbours", "neigh" );

    layout->addLayout( vlayout );
    layout->addWidget( del = new QPushButton("X") );
    m_image->setScaledContents( true );
    m_label->setText( username );
    connect(del, SIGNAL(clicked()), SIGNAL(deleteClicked()));
}

//    QString op = m_combo->itemData(m_combo->currentIndex()).toString();

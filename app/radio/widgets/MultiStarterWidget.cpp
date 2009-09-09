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
#include <QCheckBox>
#include <QTabWidget>
#include <QGridLayout>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTreeWidgetItem>
#include <lastfm/User>
#include <lastfm/XmlQuery>
#include "MultiStarterWidget.h"
#include "SourceSelectorWidget.h"
#include "SourceListWidget.h"
#include "../SourceListModel.h"
#include "../layouts/SideBySideLayout.h"
#include "SearchBox.h"
#include "YouListWidget.h"

#include <QGroupBox>

class LabelledSlider : public QSlider
{
public:
    LabelledSlider(const QString& leftText, const QString& rightText)
        : QSlider(Qt::Horizontal)
    {
        QLayout* layout = new QHBoxLayout(this);
        layout->addWidget(new QLabel(leftText));
//        layout->addWidget(this);
        layout->addWidget(new QLabel(rightText));
    }
};


MultiStarterWidget::MultiStarterWidget(bool advanced, int maxSources, QWidget *parent)
    : StylableWidget(parent)
    , m_minTagCount(10)
    , m_minArtistCount(10)
{
    QGridLayout* grid = new QGridLayout(this);

    QHBoxLayout* titleLayout = new QHBoxLayout();
    QCheckBox* checkbox;
    titleLayout->addWidget(new QLabel("Choose up to " + QString::number(maxSources) + " items and press play."), 0, Qt::AlignCenter);
    titleLayout->addWidget(checkbox = new QCheckBox("Show options"), 0, Qt::AlignRight);
    connect(checkbox, SIGNAL(stateChanged(int)), SLOT(onCheckBox(int)));
    checkbox->setChecked(advanced);
    
    QTabWidget* tabwidget = new QTabWidget();

    {
        m_youWidget = new QWidget(this);
        SideBySideLayout* layout = new SideBySideLayout(m_youWidget);
        YouListWidget* you = new YouListWidget(lastfm::ws::Username, this);
        connect(you, SIGNAL(itemActivated(QTreeWidgetItem*, int)), SLOT(onYouItemActivated(QTreeWidgetItem*, int)));
        layout->addWidget(you);
        tabwidget->addTab(m_youWidget, tr("You"));
    }

    m_artists = new SourceSelectorWidget(new ArtistSearch());    
    tabwidget->addTab(m_artists, tr("Top Artists"));
    connect(m_artists, SIGNAL(add(QString)), SLOT(onAdd(QString)));
    connect(m_artists, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(onAddItem(QListWidgetItem*)));

    m_tags = new SourceSelectorWidget(new TagSearch());
    tabwidget->addTab(m_tags, tr("Top Tags"));
    connect(m_tags, SIGNAL(add(QString)), SLOT(onAdd(QString)));
    connect(m_tags, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(onAddItem(QListWidgetItem*)));

/*
    m_users = new SourceSelectorWidget(new UserSearch());
    tabwidget->addTab(m_users, tr("Friends"));
    connect(m_users, SIGNAL(add(QString)), SLOT(onAdd(QString)));
    connect(m_users, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(onAddItem(QListWidgetItem*)));
*/

    m_sourceModel = new SourceListModel(maxSources, this);
    m_sourceList = new SourceListWidget(this);
    m_sourceList->setModel(m_sourceModel);

    QVBoxLayout* rightside = new QVBoxLayout();
    rightside->addWidget(m_sourceList);

    QVBoxLayout* sliderslayout = new QVBoxLayout();
    {
        QLabel* label = new QLabel(tr("Type of tracks played"));
        label->setObjectName("sliderDescription");
        sliderslayout->addWidget(label, Qt::AlignCenter);       // although it doesn't seem to center :(
        QLayout* layout = new QHBoxLayout(this);
        layout->addWidget(new QLabel(tr("Popular")));
        layout->addWidget(m_mainstrSlider = new QSlider(Qt::Horizontal));
        layout->addWidget(new QLabel(tr("Obscure")));
        sliderslayout->addLayout(layout);
    }
    {
        QLabel* label = new QLabel(tr("Tracks from the same artist"));
        label->setObjectName("sliderDescription");
        sliderslayout->addWidget(label, Qt::AlignCenter);
        QLayout* layout = new QHBoxLayout(this);
        layout->addWidget(new QLabel(tr("More")));
        layout->addWidget(m_repSlider = new QSlider(Qt::Horizontal));
        layout->addWidget(new QLabel(tr("Less")));
        sliderslayout->addLayout(layout);
    }
    
    m_sliders = new QWidget();
    m_sliders->setLayout(sliderslayout);
    m_sliders->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));

    m_repSlider->setMinimum(0);
    m_repSlider->setMaximum(8);
    m_repSlider->setValue(4);
    m_mainstrSlider->setMinimum(0);
    m_mainstrSlider->setMaximum(8);
    m_mainstrSlider->setValue(4);
    rightside->addWidget(m_sliders);
    rightside->addWidget(m_disco = new QCheckBox(tr("Discovery mode")));
    rightside->addWidget(m_playButton = new QPushButton(tr("Play combo")));

    grid->addLayout(titleLayout, 0, 0, 1, 2);
    grid->addWidget(tabwidget, 1, 0);
    grid->addLayout(rightside, 1, 1);
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 1);

    connect(m_playButton, SIGNAL(clicked()), SLOT(onPlayClicked()));

    //lastfm::AuthenticatedUser you;
    //connect(you.getTopTags(), SIGNAL(finished()), SLOT(onUserGotTopTags()));
    //connect(you.getTopArtists(), SIGNAL(finished()), SLOT(onUserGotTopArtists()));
    //connect(you.getFriends(), SIGNAL(finished()), SLOT(onUserGotFriends()));
    connect(Tag::getTopTags(), SIGNAL(finished()), SLOT(onGotTopTags()));

    onCheckBox(checkbox->checkState());
}

// "show options" checkbox:
void
MultiStarterWidget::onCheckBox(int checkState)
{
    m_sliders->setVisible(checkState == Qt::Checked);
    m_sourceList->updateAdvanced(checkState);
    m_disco->setVisible(checkState == Qt::Checked);
}

void
MultiStarterWidget::onAdd(const QString& item, const QString& imgUrl)
{
    RqlSource::Type type;
    if (m_artists == sender()) {
        type = RqlSource::SimArt;
    } else if (m_tags == sender()) {
        type = RqlSource::Tag;
    } else if (m_users == sender()) {
        type = RqlSource::User;
    } else {
        return;
    }

    if (m_sourceModel->addSource(RqlSource(type, item, QString(), 1.0, imgUrl))) 
    {
        // todo: grey it out if it's in the list?  or grey it some other way?
    }
}

void 
MultiStarterWidget::onAddItem(QListWidgetItem* item)
{
    onAdd(
        item->data(Qt::DisplayRole).toString(), 
        item->data(Qt::DecorationRole).toString());
}

void
MultiStarterWidget::onYouItemActivated(QTreeWidgetItem* i, int)
{
    if (i->isDisabled())
        return;             // don't know how it gets activated, but it does...

    QVariant vType = i->data(0, SourceListModel::SourceType);
    if (vType.isNull())
        return;

    RqlSource::Type sourceType = (RqlSource::Type) vType.toInt();
    if (sourceType == RqlSource::User) {
        // may be the "current" user, or a friend... a friend
        // user slides forward to the friend's profile
        QString activatedUsername = i->data(0, SourceListModel::Arg1).toString();
        YouListWidget* you = (YouListWidget*) sender();
        if (activatedUsername != you->username()) {
            // make a profile widget with a back button
            QWidget* w = new QWidget(this);
            QLayout* l = new QVBoxLayout(w);
            QPushButton* b = new QPushButton("back");
            connect(b, SIGNAL(clicked()), SLOT(onYouBack()));
            l->addWidget(b);
            YouListWidget* ylw = new YouListWidget(activatedUsername, this);
            connect(ylw, SIGNAL(itemActivated(QTreeWidgetItem*, int)), SLOT(onYouItemActivated(QTreeWidgetItem*, int)));
            l->addWidget(ylw);

            // slide m_youWidget forward to show the new profile
            m_youWidget->layout()->addWidget(w);
            ((SideBySideLayout*)m_youWidget->layout())->moveForward();
            return;
        }
    }

    m_sourceModel->addSource(RqlSource(
        sourceType,
        i->data(0, SourceListModel::Arg1).toString(),
        i->data(0, SourceListModel::Arg2).toString(),
        1.0,
        i->data(0, SourceListModel::ImageUrl).toString()));     
}

void
MultiStarterWidget::onYouBack()
{
    SideBySideLayout* layout = (SideBySideLayout*) m_youWidget->layout();
    QWidget* top = layout->currentWidget();
    layout->moveBackward();
    layout->removeWidget(top);
    top->deleteLater();
}

void
MultiStarterWidget::onGotTopTags()
{
    sender()->deleteLater();
    QNetworkReply* r = (QNetworkReply*)sender();
    lastfm::XmlQuery lfm(r->readAll());

    QStringList tags;
    foreach (lastfm::XmlQuery e, lfm["toptags"].children("tag")) {
        tags += e["name"].text();
    }
    m_tags->list()->insertItems(0, tags);
}

void
MultiStarterWidget::onUserGotTopArtists()
{
    sender()->deleteLater();
    QNetworkReply* r = (QNetworkReply*)sender();
    lastfm::XmlQuery lfm(r->readAll());

    foreach (lastfm::XmlQuery e, lfm["topartists"].children("artist")) {
        QListWidgetItem* item = new QListWidgetItem(m_artists->list());
        item->setData(Qt::DisplayRole, e["name"].text());
        item->setData(Qt::DecorationRole, e["image size=small"].text());
    }
    if (m_artists->list()->count() < m_minArtistCount) {
        // get global top artists
    }
}

void
MultiStarterWidget::onUserGotFriends()
{
    sender()->deleteLater();
    QNetworkReply* r = (QNetworkReply*)sender();
    lastfm::XmlQuery lfm(r->readAll());

    foreach (lastfm::XmlQuery e, lfm["friends"].children("user")) {
        QListWidgetItem* item = new QListWidgetItem(m_users->list());
        item->setData(Qt::DisplayRole, e["name"].text());
        item->setData(Qt::ToolTipRole, e["realname"].text());
        item->setData(Qt::DecorationRole, e["image size=small"].text());
    }
    if (m_users->list()->count() < m_minArtistCount) {
        // no friends. so?
    }
}

void
MultiStarterWidget::onPlayClicked()
{
    QString rql = m_sourceList->rql();
    if (rql.length()) {
        float r = m_repSlider->value() / (float) m_repSlider->maximum();
        if (r != 0.5) {
            rql += QString(" opt:rep|%1").arg(r);
        }
        float m = m_mainstrSlider->value() / (float) m_mainstrSlider->maximum();
        if (m != 0.5) {
            rql += QString(" opt:mainstr|%1").arg(m);
        }
        if (m_disco->isChecked()) {
            rql += QString(" opt:discovery|true");
        }
        qDebug() << rql;
        RadioStation rs = RadioStation::rql(rql);
        rs.setTitle(m_sourceList->stationDescription());
        emit startRadio(rs);
    }
}

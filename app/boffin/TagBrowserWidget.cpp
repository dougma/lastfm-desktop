/*
   Copyright 2009 Last.fm Ltd. 
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
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QItemSelectionModel>
#include "TagBrowserWidget.h"
#include "TagCloudView.h"
#include "PlaydarTagCloudModel.h"
#include "TagDelegate.h"
#include "HistoryWidget.h"
#include "PlaylistWidget.h"
#include "PlaylistModel.h"

TagBrowserWidget::TagBrowserWidget(PlaydarConnection* playdar, QWidget* parent) :
	QWidget(parent), 
    m_playdar(playdar) 
{
	m_tagCloudModel = new PlaydarTagCloudModel(playdar);
    m_filter = new RelevanceFilter();
    m_filter->setSourceModel(m_tagCloudModel);

	QVBoxLayout* vlayout = new QVBoxLayout(this);
	m_history = new HistoryWidget();
	m_history->newItem("all");
	connect(m_history, SIGNAL(clicked(int, QString)),
			SLOT(onHistoryClicked(int, QString)));
    QPushButton* button = new QPushButton("filter irrelevant tags");
    connect(button, SIGNAL(clicked()), SLOT(onFilterClicked()));
    vlayout->addWidget(button);
	vlayout->addWidget(m_history);

	QWidget* w = new QWidget(this);

	m_view = new TagCloudView(w);
	m_view->setModel(m_filter);

    connect(m_tagCloudModel, SIGNAL(tagItem(BoffinTagItem)), m_view, SLOT(onTag(BoffinTagItem)));
    connect(m_tagCloudModel, SIGNAL(fetchedTags()), m_view, SLOT(onFetchedTags()));

	m_view->setItemDelegate(new TagDelegate);
	connect(m_view->selectionModel(),
			SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
			SLOT(onSelectionChanged(QItemSelection, QItemSelection)));
	vlayout->addWidget(m_view);

// not interested in the playlist widget right now:
//	m_playlistModel = new PlaylistModel(this);
//	m_playlistWidget = new PlaylistWidget(m_playdar, this);
//	m_playlistWidget->setModel(m_playlistModel);
//	vlayout->addWidget(m_playlistWidget);

	m_tagCloudModel->startGetTags();

	this->setLayout(vlayout);
}

QStringList 
TagBrowserWidget::selectedTags() const
{
	QStringList tags;
    foreach(const QModelIndex& i, m_view->selectionModel()->selectedIndexes()) {
    	tags << i.data().toString();
    }
    return tags;
}

QString 
TagBrowserWidget::rql() 
{
    QStringList tags;
    foreach(const QString& s, selectedTags()) {
    	tags << "tag:\"" + s + '"';
    }
	return tags.join(" and ");
}

void TagBrowserWidget::onSelectionChanged(const QItemSelection& selected,
		const QItemSelection& deselected) 
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);

    QString rql = TagBrowserWidget::rql();
    qDebug() << "filtering: " << rql;
    m_filter->setRqlFilter(m_playdar, rql, m_view->selectionModel()->selectedIndexes());

//    ((PlaylistModel*)m_playlistWidget->model())->clear();
//    m_playlistWidget->loadFromRql(rql);

    emit selectionChanged();
}


void 
TagBrowserWidget::onHistoryClicked(int position, const QString& text) {
	Q_UNUSED(text);

	// move backward to the clicked-button
	while (m_tags.size() > position) {
		// the view is deleted after the animation finishes
		QString t = m_tags.takeLast();
		QModelIndex i = m_tagCloudModel->indexOf(BoffinTagItem(t));
		m_view->selectionModel()->select(i, QItemSelectionModel::Deselect);
		m_view->update(i);
		m_history->pop();
	}
	emit selectionChanged();
}

void
TagBrowserWidget::onFilterClicked()
{
    // toggle the filter:
    m_filter->showRelevant(m_filter->showingAll());
}
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
#include <QSlider>
#include <QLabel>
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

	QWidget* w = new QWidget(this);

	m_rqlSentence = new QLabel( this );
	vlayout->addWidget( m_rqlSentence );
	
	m_view = new TagCloudView(w);
	m_view->setModel(m_filter);

    connect(m_tagCloudModel, SIGNAL(tagItem(BoffinTagItem)), m_view, SLOT(onTag(BoffinTagItem)));
    connect(m_tagCloudModel, SIGNAL(fetchedTags()), m_view, SLOT(onFetchedTags()));

	m_view->setItemDelegate(new TagDelegate);
	connect(m_view->selectionModel(),
			SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
			SLOT(onSelectionChanged(QItemSelection, QItemSelection)));
	vlayout->addWidget(m_view);

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
    if( !selected.isEmpty() )
    if( qFuzzyCompare( 0.0f, selected.last().topLeft().data( PlaydarTagCloudModel::RelevanceRole ).value<float>() ) )
        qDebug() << "Need to OR!";
    else
        qDebug() << "No Need to OR: " << selected.last().topLeft().data( PlaydarTagCloudModel::RelevanceRole ).value<float>() << "!";
    
    m_filter->setRqlFilter(m_playdar, rql, m_view->selectionModel()->selectedIndexes());

    m_rqlSentence->setText( rql );
    
//    ((PlaylistModel*)m_playlistWidget->model())->clear();
//    m_playlistWidget->loadFromRql(rql);

    emit selectionChanged();
}

void
TagBrowserWidget::onFilterClicked()
{
    // toggle the filter:
    m_filter->showRelevant(m_filter->showingAll());
}

void
TagBrowserWidget::onSliderChanged( int val )
{
	m_filter->setMinimumTrackCountFilter( val );
}

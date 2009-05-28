#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
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

	m_playlistModel = new PlaylistModel(this);
	m_playlistWidget = new PlaylistWidget(m_playdar, this);
	m_playlistWidget->setModel(m_playlistModel);
	vlayout->addWidget(m_playlistWidget);

	m_tagCloudModel->startGetTags();

	this->setLayout(vlayout);
}

QString TagBrowserWidget::rql() const {
	QStringList tags;
	for (int i = 0; i < m_tags.count(); ++i)
		tags << "tag:\"" + m_tags[i] + '"';
	return tags.join(" and ");
}

void TagBrowserWidget::onSelectionChanged(const QItemSelection& selected,
		const QItemSelection& deselected) 
{
    bool changed = false;

	if (selected.indexes().size()) {
		foreach( const QModelIndex& i, selected.indexes() ) {
			QString tag = i.data().toString();
			if (m_tags.contains(tag))
				continue;

			m_tags << tag;
            changed = true;
    		m_history->newItem(tag);
		}
	}

    if (deselected.indexes().size()) {
        qDebug() << "deselection";
		foreach( const QModelIndex& i, selected.indexes() ) {
			QString tag = i.data().toString();
            if (m_tags.removeAll(tag)) {
                changed = true;
                // todo needs to remove tag from m_history
            }
        }
    }

    if (changed) {
        QString rql(rql());
        qDebug() << "filtering: " << rql;
		m_filter->setRqlFilter(m_playdar, rql);

		((PlaylistModel*)m_playlistWidget->model())->clear();
		m_playlistWidget->loadFromRql(rql);
    }

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
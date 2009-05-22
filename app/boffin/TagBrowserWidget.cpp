#include <QWidget>
#include "TagBrowserWidget.h"
#include "TagCloudView.h"
#include "PlaydarTagCloudModel.h"
#include "TagDelegate.h"
#include "layouts/SideBySideLayout.h"
#include "HistoryWidget.h"
#include "PlaylistWidget.h"
#include "PlaylistModel.h"

TagBrowserWidget::TagBrowserWidget(boost::function<PlaydarTagCloudModel* (void)> modelFactory,
                               const QString& firstButton,
                               PlaydarConnection* playdar,
                               QWidget* parent)
: QWidget(parent)
, m_modelFactory(modelFactory)
, m_playdar( playdar )
{
    QVBoxLayout* vlayout = new QVBoxLayout(this);
    m_history = new HistoryWidget();
    m_history->newItem(firstButton);
    connect(m_history, SIGNAL(clicked(int, QString)), SLOT(onHistoryClicked(int, QString)));
    vlayout->addWidget(m_history);

    QWidget* w = new QWidget( this );
    m_view = new TagCloudView( w );
    m_view->setSelectionMode( QAbstractItemView::SingleSelection );
    vlayout->addWidget( m_view );

    m_playlistModel = new PlaylistModel( this );
    m_playlistWidget = new PlaylistWidget( m_playdar, this );
    m_playlistWidget->setModel( m_playlistModel );
    vlayout->addWidget( m_playlistWidget );

    setupModelView( m_view );

    this->setLayout(vlayout);
}

QString
TagBrowserWidget::rql() const
{
    QStringList tags;
    for (int i = 0; i < m_tags.count(); ++i)
        tags << "tag:\"" + m_tags[i] + '"';
    return tags.join(" and ");
}

void
TagBrowserWidget::onSelectionChanged( const QItemSelection& selected, const QItemSelection& deselected )
{
    if (selected.indexes().size()) {
    	//FIXME: wow! this needs to be done properly!
        QString tag( m_view->model()->data( selected.indexes().at(0) ).toString() );
        m_tags << tag;
        m_history->newItem( tag );
        setupModelView( m_view );

        m_playlistWidget->loadFromRql( rql() );
    }
    emit selectionChanged();
}

void
TagBrowserWidget::setupModelView(TagCloudView* view)
{
    PlaydarTagCloudModel* model = m_modelFactory();
    view->setModel( model );
    view->setItemDelegate( new TagDelegate );
    connect(
        view->selectionModel(),
        SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
        SLOT(onSelectionChanged(QItemSelection, QItemSelection)));
    model->startGetTags( rql() );
}

void
TagBrowserWidget::onHistoryClicked(int position, const QString& text)
{
    Q_UNUSED(text);

    // move backward to the clicked-button
    while(m_tags.size() > position) {
        // the view is deleted after the animation finishes
        m_tags.removeLast();
        m_history->pop();
    }
    emit selectionChanged();
}


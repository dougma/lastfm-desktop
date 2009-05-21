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
    m_layout = new SideBySideLayout();


    QVBoxLayout* vlayout = new QVBoxLayout(this);
    m_history = new HistoryWidget();
    m_history->newItem(firstButton);
    connect(m_history, SIGNAL(clicked(int, QString)), SLOT(onHistoryClicked(int, QString)));
    connect( m_layout, SIGNAL(animationFinished()), SLOT(onAnimationFinished()));
    vlayout->addWidget(m_history);

    QWidget* w = new QWidget( this );
    new QVBoxLayout( w );

    TagCloudView* view( new TagCloudView( w ) );
    view->setSelectionMode( QAbstractItemView::SingleSelection );
    w->layout()->addWidget( view );

    PlaylistWidget* playlist = new PlaylistWidget( m_playdar, w );
    m_playlist = new PlaylistModel( playlist );
    playlist->setModel( m_playlist );
    w->layout()->addWidget( playlist );


    m_layout->addWidget( w );

    setupModelView( view );

    vlayout->addLayout(m_layout);
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
        TagCloudView* topView = static_cast<TagCloudView*>( m_layout->currentWidget()->layout()->itemAt(0)->widget() );
        QString tag( topView->model()->data( selected.indexes().at(0) ).toString() );
        m_tags << tag;

        m_history->newItem( tag );

        QWidget* w = new QWidget( this );
        new QVBoxLayout( w );

        TagCloudView* view( new TagCloudView( w ) );
        view->setSelectionMode( QAbstractItemView::SingleSelection );
        w->layout()->addWidget( view );

        PlaylistWidget* playlist = new PlaylistWidget( m_playdar, w );
        playlist->loadFromRql( rql() );
        w->layout()->addWidget( playlist );

        m_layout->addWidget( w );
        m_layout->moveForward();
        setupModelView( view );
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
        m_layout->moveBackward();
        // the view is deleted after the animation finishes
        m_tags.removeLast();
        m_history->pop();
    }
    emit selectionChanged();
}

void
TagBrowserWidget::onAnimationFinished()
{
    // remove anything 'forward' of the current widget
    int current = m_layout->currentItemIndex();
    for (int i = m_layout->count() - 1; i > current; i--) {
    	delete m_layout->takeAt(i)->widget();
    }
}

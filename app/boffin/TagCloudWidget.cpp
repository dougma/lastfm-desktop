#include <QWidget>
#include "TagCloudWidget.h"
#include "TagCloudView.h"
#include "PlaydarTagCloudModel.h"
#include "TagDelegate.h"
#include "layouts/SideBySideLayout.h"
#include "HistoryWidget.h"


TagCloudWidget::TagCloudWidget(boost::function<PlaydarTagCloudModel* (void)> modelFactory, 
                               const QString& firstButton,
                               QWidget* parent)
: QWidget(parent)
, m_modelFactory(modelFactory)
{
    m_layout = new SideBySideLayout();

    QVBoxLayout* vlayout = new QVBoxLayout(this);
    m_history = new HistoryWidget();
    m_history->newItem(firstButton);
    connect(m_history, SIGNAL(clicked(int, QString)), SLOT(onHistoryClicked(int, QString)));
    vlayout->addWidget(m_history);

    TagCloudView* view( new TagCloudView() );
    m_layout->addWidget( view );
    setupModelView( view );

    vlayout->addLayout(m_layout);
}

QString
TagCloudWidget::rql() const
{
    QStringList tags;
    for (int i = 0; i < m_tags.count(); ++i)
        tags << "tag:\"" + m_tags[i] + '"';
    return tags.join(" and ");
}

void
TagCloudWidget::onSelectionChanged( const QItemSelection& selected, const QItemSelection& deselected )
{
    if (selected.indexes().size()) {
        TagCloudView* topView = static_cast<TagCloudView*>( m_layout->currentWidget() );
        QString tag( topView->model()->data( selected.indexes().at(0) ).toString() );
        m_tags << tag;

        m_history->newItem( tag );

        TagCloudView* view( new TagCloudView() );
        m_layout->addWidget( view );
        m_layout->moveForward();
        setupModelView( view );
    }
    emit selectionChanged();
}

void
TagCloudWidget::setupModelView(TagCloudView* view)
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
TagCloudWidget::onHistoryClicked(int position, const QString& text)
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
TagCloudWidget::onAnimationFinished()
{
    // remove anything 'forward' of the current widget
    int current = m_layout->currentItemIndex();
    for (int i = m_layout->count(); i > current; i--) {
        delete m_layout->takeAt(i);
    }
}
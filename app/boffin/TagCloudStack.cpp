#include <QLabel>
#include <QLayout>
#include "TagCloudStack.h"
#include "TagCloudView.h"
#include "PlaydarTagCloudModel.h"
#include "TagDelegate.h"


TagCloudStack::TagCloudStack(boost::function<PlaydarTagCloudModel* (void)> modelFactory, QWidget* parent)
: QStackedWidget(parent)
, m_modelFactory(modelFactory)
{
    TagCloudView* tcv( new TagCloudView() );
    addWidget( tcv );
    setupModelView( tcv );

    QLabel* m_label = new QLabel();
    m_label->setText("testing");
    this->layout()->addWidget(m_label);
}

QString
TagCloudStack::rql() const
{
    QStringList tags;
    for (int i = 0; i < m_tags.count(); ++i)
        tags << "tag:\"" + m_tags[i] + '"';
    return tags.join(" and ");
}


void 
TagCloudStack::pop()
{
    removeWidget( widget( m_tags.size() ) );
    m_tags.removeLast();
}

TagCloudView*
TagCloudStack::getFirstView() const
{
    return static_cast<TagCloudView*>( widget( 0 ) );
}

void
TagCloudStack::onSelectionChanged( const QItemSelection& selected, const QItemSelection& deselected )
{
    if (selected.indexes().size()) {
        TagCloudView* topView = static_cast<TagCloudView*>( currentWidget() );
        QString tag( topView->model()->data( selected.indexes().at(0) ).toString() );
        m_tags << tag;

        TagCloudView* view( new TagCloudView() );
        addWidget( view );
        setCurrentWidget( view );
        setupModelView( view );

        emit tagsChange(m_tags);
    }
}

void
TagCloudStack::setupModelView(TagCloudView* view)
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

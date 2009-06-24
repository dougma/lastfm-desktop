// this class can probably be done better

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QLabel>
#include <QSpacerItem>
#include "SourceListWidget.h"
#include "lib/unicorn/widgets/ImageButton.h"



SourceListWidget::SourceListWidget(int maxSources, QWidget* parent)
: QWidget(parent)
, m_maxSources(maxSources)
{
    m_layout = new QVBoxLayout(this);
    for (int i = 0; i < maxSources; i++) {
        addPlaceholder();
    }
}

void
SourceListWidget::addPlaceholder()
{
    if (m_layout->itemAt(0)) {
        QComboBox* combo = new QComboBox();
        combo->setDisabled(true);
        m_layout->addWidget(combo);
    }
    QGroupBox* box = new QGroupBox("");
    m_layout->addWidget(box);
}

bool 
SourceListWidget::addSource(SourceType type, const QString& name)
{
    if (m_sources.size() == m_maxSources)
        return false;

    m_sources.append(Source(type, name));
    int idx = m_sources.size() - 1;
    setSource(idx, createWidget(type, name));
    setOp(idx);
    return true;
}

void
SourceListWidget::setSource(int sourceIdx, QWidget* widget)
{
    QGroupBox* box = qobject_cast<QGroupBox*>(m_layout->itemAt(sourceIdx * 2)->widget());
    widget->setParent(box);
    widget->show();

    // todo: how would i float this in the center of the right edge of the box?
//    ImageButton* button = new ImageButton("C:\\dev\\lastfm-desktop\\app\\radio\\widgets\\delete.png");
//    button->setParent(box);
//    button->show();
}


void
SourceListWidget::setOp(int sourceIdx)
{
    if (sourceIdx > 0) {
        QComboBox* combo = qobject_cast<QComboBox*>(m_layout->itemAt(sourceIdx * 2 - 1)->widget());
        combo->setEnabled(true);
        combo->addItems(QStringList() << "and" << "or" << "and not");
        Operator op = defaultOp(m_sources[sourceIdx-1].first, m_sources[sourceIdx].first);
        switch (op) {
            case And: combo->setCurrentIndex(0); break;
            case Or: combo->setCurrentIndex(1); break;
            case AndNot: combo->setCurrentIndex(2); break;
        }
    }
}

//static
QWidget*
SourceListWidget::createWidget(SourceType type, const QString& name)
{
    switch (type) {
        case Artist: return new QLabel("Artist: " + name);
        case Tag: return new QLabel("Tag: " + name);
        case User: return new QLabel("User: " + name);
    }
    return 0;
}

//static
SourceListWidget::Operator
SourceListWidget::defaultOp(SourceListWidget::SourceType first, SourceListWidget::SourceType second)
{
    return (first == Tag && second == Tag) ? And : Or;
}
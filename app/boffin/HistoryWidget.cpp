#include <QHBoxLayout>
#include <QPushButton>
#include "HistoryWidget.h"

HistoryWidget::HistoryWidget(QWidget* parent)
    : QWidget(parent)
{
    m_layout = new QHBoxLayout(this);
}

HistoryWidget::~HistoryWidget()
{
    while (pop())
        ;
}

bool 
HistoryWidget::pop()
{
    if (m_labels.count()) {
        QPushButton* label = m_labels.pop();
        m_layout->removeWidget(label);
        delete label;
        return true;
    }
    return false;
}


void 
HistoryWidget::newItem(const QString& text)
{
    QPushButton* label = new QPushButton(text, this);
    m_layout->addWidget(label, 0, Qt::AlignLeft);
    m_labels.push(label);
    connect(label, SIGNAL(clicked()), SLOT(onItemClicked()));
}

void 
HistoryWidget::onItemClicked()
{
    QPushButton* label = static_cast<QPushButton*>(sender());
    for (int i = 0; i < m_labels.count(); i++) {
        if (m_labels[i] == label) {
            emit clicked(i, label->text());
            return;
        }
    }
}

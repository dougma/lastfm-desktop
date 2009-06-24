#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QListWidgetItem>
#include "SourceSelectorWidget.h"

SourceSelectorWidget::SourceSelectorWidget(QLineEdit* edit, QWidget* parent)
    :QWidget(parent)
{
    QGridLayout* grid = new QGridLayout(this);

    m_edit = edit;
    m_edit->setParent(this);
    grid->addWidget(m_edit, 0, 0);

    m_button = new QPushButton(tr("Add"));
    grid->addWidget(m_button, 0, 1);
    grid->setColumnStretch(0, 3);

    m_list = new QListWidget();
    grid->addWidget(m_list, 1, 0, 1, 2);

    connect(m_edit, SIGNAL(returnPressed()), SLOT(emitAdd())); 
    connect(m_button, SIGNAL(clicked()), SLOT(emitAdd()));
    connect(m_list, SIGNAL(itemActivated(QListWidgetItem *)), SLOT(onListItemActivated(QListWidgetItem *)));
}

QListWidget* 
SourceSelectorWidget::list()
{
    return m_list;
}

void
SourceSelectorWidget::emitAdd()
{
    emit add(m_edit->text());
}

void
SourceSelectorWidget::onListItemActivated(QListWidgetItem * item)
{
    if (item) {
        emit add(item->data(Qt::DisplayRole).toString());
    }
}
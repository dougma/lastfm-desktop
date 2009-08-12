/*
   Copyright 2005-2009 Last.fm Ltd. 

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

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QLabel>
#include <QSpacerItem>
#include <QListWidgetItem>
#include "SourceListWidget.h"
#include "SourceItemWidget.h"
#include "lib/unicorn/widgets/ImageButton.h"
#include "lastfm/ws.h"


SourceListWidget::SourceListWidget(bool advanced, QWidget* parent)
: QWidget(parent)
, m_advanced(advanced)
, m_layout(0)
{
    //m_layout = new QVBoxLayout(this);
}

void
SourceListWidget::setModel(SourceListModel* model)
{
    if (model) {
        m_model = model;
        // safe to throw away the layout like this?
        if (m_layout)
            m_layout->deleteLater();
        m_layout = new QVBoxLayout(this);
        addPlaceholders();
        connect(m_model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), SLOT(onRowsAboutToBeRemoved(QModelIndex, int, int)));
        connect(m_model, SIGNAL(rowsInserted(QModelIndex, int, int)), SLOT(onRowsInserted(QModelIndex, int, int)));
    }
}

void
SourceListWidget::addPlaceholders()
{
    while (m_layout->count() < (m_model->getMaxSize() * 2) - 1) {
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
        if (!m_advanced) {
            combo->hide();
        }
    }
    QGroupBox* box = new QGroupBox("");
    m_layout->addWidget(box);
}

void 
SourceListWidget::onRowsInserted(const QModelIndex& parent, int start, int end)
{
    Q_UNUSED(parent);
    for (int idx = start; idx <= end; idx++) {
        QWidget* old = m_layout->itemAt(idx * 2)->widget();
        m_layout->removeWidget(old);
        old->deleteLater();
        m_layout->insertWidget(idx * 2, createWidget(idx));
        setOp(idx);
    }
}

void
SourceListWidget::onRowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
    Q_UNUSED(parent);
    for (int idx = start; idx <= end; idx++) {
        bool first = idx == 0;
        int layoutIdx = idx * 2;
        if (!first) {
            // remove any preceding operator control
            layoutIdx--;
            m_layout->takeAt(layoutIdx)->widget()->deleteLater();
        }
        m_layout->takeAt(layoutIdx)->widget()->deleteLater();
        if (first) {
            m_layout->takeAt(0)->widget()->deleteLater();
        }
        addPlaceholders();
    }
}

void
SourceListWidget::setOp(int sourceIdx)
{
    if (sourceIdx > 0) {
        QComboBox* combo = qobject_cast<QComboBox*>(m_layout->itemAt(sourceIdx * 2 - 1)->widget());
        combo->setEnabled(true);
        combo->addItems(QStringList() << "and" << "or" << "not");
        RqlSource::Type src1 = (RqlSource::Type) m_model->data(m_model->index(sourceIdx-1), SourceListModel::SourceType).toInt();
        RqlSource::Type src2 = (RqlSource::Type) m_model->data(m_model->index(sourceIdx), SourceListModel::SourceType).toInt();
        Operator op = defaultOp(src1, src2);
        switch (op) {
            case And: combo->setCurrentIndex(0); break;
            case Or: combo->setCurrentIndex(1); break;
            case AndNot: combo->setCurrentIndex(2); break;
        }
    }
}

SourceItemWidget*
SourceListWidget::createWidget(int idx)
{
    QModelIndex qidx = m_model->index(idx);
    RqlSource::Type type = (RqlSource::Type) m_model->data(qidx, SourceListModel::SourceType).toInt();
    QVariant arg1 = m_model->data(qidx, SourceListModel::Arg1);
//    QVariant arg2 = m_model->data(qidx, SourceListModel::Arg2);
    SourceItemWidget* result = 0;
    switch (type) {
        case RqlSource::SimArt: 
            result = new SourceItemWidget(arg1.toString());
            break;
        case RqlSource::Tag: 
            result = new SourceItemWidget(arg1.toString());
            break;
        case RqlSource::User: 
            {
                UserItemWidget* widget = new UserItemWidget(arg1.toString());
                widget->setModel(m_model, m_model->index(idx, 0));
                result = widget;
            }
            break;
    }
    if (result) {
        QString imgUrl = m_model->data(qidx, SourceListModel::ImageUrl).toString();
        if (imgUrl.length()) {
            result->getImage(QUrl(imgUrl));
        }
        connect(result, SIGNAL(deleteClicked()), SLOT(onDeleteClicked()));
    }
    return result;
}

void
SourceListWidget::onDeleteClicked()
{
    SourceItemWidget* source = qobject_cast<SourceItemWidget*>(sender());
    if (source) {
        int idx = m_layout->indexOf(source);
        if (idx != -1) {
            m_model->removeSource(idx / 2);
        }
    }
}

//static
SourceListWidget::Operator
SourceListWidget::defaultOp(RqlSource::Type first, RqlSource::Type second)
{
    return (first == RqlSource::Tag && second == RqlSource::Tag) ? And : Or;
}

QString
SourceListWidget::rql()
{
    int count = 0;
    QString result;
    foreach (const QString& src, m_model->rql()) {
        if (count) {
            // get operator
            QComboBox* combo = qobject_cast<QComboBox*>(m_layout->itemAt(count * 2 - 1)->widget());
            switch (combo->currentIndex()) {
                case 0: result += " and "; break;
                case 1: result += " or "; break;
                case 2: result += " not "; break;
            }
        }
        result += src;
        count++;
    }
    return result;
}

QString
SourceListWidget::stationDescription()
{
    int count = 0;
    QString result;
    foreach (const QString& src, m_model->descriptions()) {
        if (count) {
            // get operator
            QComboBox* combo = qobject_cast<QComboBox*>(m_layout->itemAt(count * 2 - 1)->widget());
            switch (combo->currentIndex()) {
                case 0: result += " and "; break;
                case 1: result += " or "; break;
                case 2: result += " not "; break;
            }
        }
        result += src;
        count++;
    }
    return result;
}

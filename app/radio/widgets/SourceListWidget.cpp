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


// this class can probably be done better

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
SourceListWidget::addPlaceholders()
{
    while (m_layout->count() < (m_maxSources * 2) - 1) {
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
SourceListWidget::sourceInList(SourceType type, const QString& name)
{
    return m_sources.contains(Source(type, name));
}

bool 
SourceListWidget::addSource(SourceType type, const QString& name)
{
    if (m_sources.size() == m_maxSources || sourceInList(type, name))
        return false;

    m_sources.append(Source(type, name));
    int idx = m_sources.size() - 1;
    QWidget* old = m_layout->itemAt(idx * 2)->widget();
    m_layout->removeWidget(old);
    old->deleteLater();
    m_layout->insertWidget(idx * 2, createWidget(type, name));
    setOp(idx);
    return true;
}

bool 
SourceListWidget::addSource(SourceType type, QListWidgetItem* item)
{
    QString name = item->data(Qt::DisplayRole).toString();
    if (m_sources.size() == m_maxSources || sourceInList(type, name))
        return false;

    SourceItemWidget* sourceItemWidget = createWidget(type, name);

    m_sources.append(Source(type, name));
    int idx = m_sources.size() - 1;
    QWidget* old = m_layout->itemAt(idx * 2)->widget();
    m_layout->removeWidget(old);
    old->deleteLater();
    m_layout->insertWidget(idx * 2, sourceItemWidget);
    setOp(idx);

    QString imageUrl = item->data(Qt::DecorationRole).toString();
    if (imageUrl.length()) {
        QNetworkReply* reply = lastfm::nam()->get(QNetworkRequest(imageUrl));
        connect(reply, SIGNAL(finished()), sourceItemWidget, SLOT(onGotImage()));
    }
    return true;
}

void
SourceListWidget::setOp(int sourceIdx)
{
    if (sourceIdx > 0) {
        QComboBox* combo = qobject_cast<QComboBox*>(m_layout->itemAt(sourceIdx * 2 - 1)->widget());
        combo->setEnabled(true);
        combo->addItems(QStringList() << "and" << "or" << "not");
        Operator op = defaultOp(m_sources[sourceIdx-1].first, m_sources[sourceIdx].first);
        switch (op) {
            case And: combo->setCurrentIndex(0); break;
            case Or: combo->setCurrentIndex(1); break;
            case AndNot: combo->setCurrentIndex(2); break;
        }
    }
}

SourceItemWidget*
SourceListWidget::createWidget(SourceType type, const QString& name)
{
    SourceItemWidget* result = 0;
    switch (type) {
        case Artist: 
            result = new SourceItemWidget("Artist: " + name);
            break;
        case Tag: 
            result = new SourceItemWidget("Tag: " + name);
            break;
        case User: 
            result = new SourceItemWidget("User: " + name);
            break;
        default:
            return 0;
    }
    connect(result, SIGNAL(deleteClicked()), SLOT(onDeleteClicked()));
    return result;
}

void
SourceListWidget::onDeleteClicked()
{
    sender()->deleteLater();
    SourceItemWidget* source = qobject_cast<SourceItemWidget*>(sender());
    if (source) {
        int idx = m_layout->indexOf(source);
        if (idx != -1) {
            m_sources.removeAt(idx / 2);
            bool first = idx == 0;
            if (!first) {
                // remove any preceding operator control
                idx--;
                m_layout->takeAt(idx)->widget()->deleteLater();
            }
            m_layout->takeAt(idx)->widget()->deleteLater();
            if (first) {
                m_layout->takeAt(0)->widget()->deleteLater();
            }
            addPlaceholders();
        }
    }
}

//static
SourceListWidget::Operator
SourceListWidget::defaultOp(SourceListWidget::SourceType first, SourceListWidget::SourceType second)
{
    return (first == Tag && second == Tag) ? And : Or;
}

QString
SourceListWidget::rql()
{
    int count = 0;
    QString result;
    foreach (const Source& src, m_sources) {
        QString rqlSource;
        switch(src.first) {
            case Artist: rqlSource = "simart:"; break;
            case Tag: rqlSource = "tag:"; break;
            case User: rqlSource = "library:"; break;
        }

        if (count) {
            // get operator
            QComboBox* combo = qobject_cast<QComboBox*>(m_layout->itemAt(count * 2 - 1)->widget());
            switch (combo->currentIndex()) {
                case 0: result += " and "; break;
                case 1: result += " or "; break;
                case 2: result += " not "; break;
            }
        }
        result += rqlSource + "\"" + src.second + "\"";
        count++;
    }
    return result;
}

QString
SourceListWidget::stationDescription()
{
    int count = 0;
    QString result;
    foreach (const Source& src, m_sources) {
        if (count) {
            // get operator
            QComboBox* combo = qobject_cast<QComboBox*>(m_layout->itemAt(count * 2 - 1)->widget());
            switch (combo->currentIndex()) {
                case 0: result += " and "; break;
                case 1: result += " or "; break;
                case 2: result += " not "; break;
            }
        }
        result += src.second;
        count++;
    }
    return result;
}

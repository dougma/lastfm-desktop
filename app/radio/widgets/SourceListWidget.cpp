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
SourceListWidget::addPlaceholder()
{
    if (m_layout->itemAt(0)) {
        QComboBox* combo = new QComboBox();
        combo->setDisabled(true);
        m_layout->addWidget(combo);
    }
    QGroupBox* box = new QGroupBox("");
    box->setLayout(new QHBoxLayout());
    m_layout->addWidget(box);
}

bool 
SourceListWidget::addSource(SourceType type, const QString& name)
{
    if (m_sources.size() == m_maxSources)
        return false;

    m_sources.append(Source(type, name));
    int idx = m_sources.size() - 1;
    QLayoutItem* li = m_layout->itemAt(idx * 2);
    QLayout* layout = ((QGroupBox *)li->widget())->layout();
    createWidget(type, name, layout);
    setOp(idx);
    return true;
}

bool 
SourceListWidget::addSource(SourceType type, QListWidgetItem* item)
{
    if (m_sources.size() < m_maxSources) {
        QString name = item->data(Qt::DisplayRole).toString();
        if (name.length() > 0) {
            m_sources.append(Source(type, name));
            int idx = m_sources.size() - 1;
            QLayoutItem* li = m_layout->itemAt(idx * 2);
            QLayout* layout = ((QGroupBox *)li->widget())->layout();
            SourceItemWidget* sourceItemWidget = createWidget(type, name, layout);
            setOp(idx);

            QString imageUrl = item->data(Qt::DecorationRole).toString();
            if (imageUrl.length()) {
                QNetworkReply* reply = lastfm::nam()->get(QNetworkRequest(imageUrl));
                connect(reply, SIGNAL(finished()), sourceItemWidget, SLOT(onGotImage()));
            }
            return true;
        }
    }
    return false;
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

//static
SourceItemWidget*
SourceListWidget::createWidget(SourceType type, const QString& name, QLayout* layout)
{
    switch (type) {
        case Artist: return new SourceItemWidget("Artist: " + name, layout);
        case Tag: return new SourceItemWidget("Tag: " + name, layout);
        case User: return new SourceItemWidget("User: " + name, layout);
    }
    return 0;
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

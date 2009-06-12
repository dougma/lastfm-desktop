/***************************************************************************
 *   Copyright 2009 Last.fm Ltd.                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QLayout>
#include <QTreeWidget>
#include <QHBoxLayout>
#include "XspfDialog.h"
#include "XspfReader.h"
#include "playdar/PlaydarConnection.h"
#include "playdar/TrackResolveRequest.h"
#include "playdar/BoffinPlayableItem.h"


XspfDialog::XspfDialog(QString path, PlaydarConnection* playdar, QWidget *parent)
:QDialog(parent)
,m_playdar(playdar)
{
    QLayout* layout = new QHBoxLayout();
    m_treewidget = new QTreeWidget();
    m_treewidget->setColumnCount(12);
    QStringList labels;
    labels << "Position" << "Artist" << "Album" << "Title" << "Length" << "Url"
        << "Score" << "Pref" << "Source" << "Bitrate" << "Size" << "Mimetype";
    m_treewidget->setHeaderLabels(labels);
    layout->addWidget(m_treewidget);
    setLayout(layout);
    setSizeGripEnabled(true);

    m_reader = new XspfReader(path);
    connect(m_reader, SIGNAL(xspf(lastfm::Xspf)), SLOT(onXspf(lastfm::Xspf)));
}

void
XspfDialog::onXspf(const lastfm::Xspf& xspf)
{
    setWindowTitle(xspf.title());
    int i = 0;
    foreach(const lastfm::Track& t, xspf.tracks()) {
        TrackResolveRequest* req = m_playdar->trackResolve(t.artist(), t.album(), t.title());

        QTreeWidgetItem* item = new QTreeWidgetItem();
        m_reqmap[req->qid()] = item;
        item->setData(0, Qt::DisplayRole, QString::number(i));
        item->setData(1, Qt::DisplayRole, (QString)t.artist());
        item->setData(2, Qt::DisplayRole, (QString)t.album());
        item->setData(3, Qt::DisplayRole, (QString)t.title());
        item->setData(4, Qt::DisplayRole, t.duration());
        item->setData(5, Qt::DisplayRole, t.url().toString());
        m_treewidget->addTopLevelItem(item);

        connect(req, SIGNAL(result(BoffinPlayableItem)), SLOT(onResolveResult(BoffinPlayableItem)));
        i++;
    }
}

void
XspfDialog::onResolveResult(const BoffinPlayableItem& item)
{
    TrackResolveRequest* req = qobject_cast<TrackResolveRequest*>(sender());
    if (req) {
        QMap<QString, QTreeWidgetItem*>::const_iterator it = m_reqmap.constFind(req->qid());
        if (it != m_reqmap.constEnd()) {
            QTreeWidgetItem* child = new QTreeWidgetItem();
            child->setFirstColumnSpanned(true);
            child->setData(1, Qt::DisplayRole, item.artist());
            child->setData(2, Qt::DisplayRole, item.album());
            child->setData(3, Qt::DisplayRole, item.track());
            child->setData(4, Qt::DisplayRole, item.duration());
            child->setData(5, Qt::DisplayRole, item.url());
            child->setData(6, Qt::DisplayRole, item.score());
            child->setData(7, Qt::DisplayRole, item.preference());
            child->setData(8, Qt::DisplayRole, item.source());
            child->setData(9, Qt::DisplayRole, item.bitrate());
            child->setData(10, Qt::DisplayRole, item.size());
            child->setData(11, Qt::DisplayRole, item.mimetype());
            it.value()->addChild(child);
        } else {
            qDebug() << "unknown qid " << req->qid();
        }
    }
}
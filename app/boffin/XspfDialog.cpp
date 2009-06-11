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
#include <QTreeView>
#include <QHBoxLayout>
#include "XspfDialog.h"
#include "XspfModel.h"
#include "XspfReader.h"
#include "playdar/PlaydarConnection.h"
#include "playdar/TrackResolveRequest.h"
#include "playdar/BoffinPlayableItem.h"

XspfDialog::XspfDialog(QString path, PlaydarConnection* playdar, QWidget *parent)
:QDialog(parent)
,m_playdar(playdar)
{
    QLayout* layout = new QHBoxLayout();
    m_treeview = new QTreeView();
    layout->addWidget(m_treeview);
    setLayout(layout);
    setSizeGripEnabled(true);

    m_reader = new XspfReader(path);
    connect(m_reader, SIGNAL(xspf(lastfm::Xspf)), SLOT(onXspf(lastfm::Xspf)));
}

void
XspfDialog::onXspf(const lastfm::Xspf& xspf)
{
    m_model = new XspfModel();
    setWindowTitle(xspf.title());
    int i = 0;
    foreach(const lastfm::Track& t, xspf.tracks()) {
        TrackResolveRequest* req = m_playdar->trackResolve(t.artist(), t.album(), t.title());
        m_reqmap[req->qid()] = i;
        m_model->addTrack(t);
        connect(req, SIGNAL(result(BoffinPlayableItem)), SLOT(onResolveResult(BoffinPlayableItem)));
        i++;
    }

    m_treeview->setModel(m_model);
}

void
XspfDialog::onResolveResult(const BoffinPlayableItem& item)
{
    TrackResolveRequest* req = qobject_cast<TrackResolveRequest*>(sender());
    if (req) {
        QMap<QString, int>::const_iterator it = m_reqmap.constFind(req->qid());
        if (it != m_reqmap.constEnd()) {
            m_model->addResult(it.value(), item);
        } else {
            qDebug() << "unknown qid " << req->qid();
        }
    }
}
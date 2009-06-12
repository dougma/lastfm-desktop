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
#ifndef XSPF_DIALOG_H
#define XSPF_DIALOG_H

#include <QDialog>
#include <QMap>
#include <lastfm/Xspf>

class QTreeWidget;
class QTreeWidgetItem;
class XspfReader;
class PlaydarConnection;
class BoffinPlayableItem;

class XspfDialog : public QDialog
{
    Q_OBJECT

public:
    XspfDialog(QString url, PlaydarConnection* playdar, QWidget *parent = 0);

private slots:
    void onXspf(const lastfm::Xspf& xspf);
    void onResolveResult(const BoffinPlayableItem& item);

private:
    XspfReader* m_reader;
    QTreeWidget* m_treewidget;
    PlaydarConnection* m_playdar;
    QMap<QString, QTreeWidgetItem*> m_reqmap;    // map request qid to index
};

#endif

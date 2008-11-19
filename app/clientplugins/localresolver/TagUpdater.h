/***************************************************************************
 *   Copyright 2007-2008 Last.fm Ltd.                                      *
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


#ifndef TAG_UPDATER_H
#define TAG_UPDATER_H

#include <QThread>
#include <QSet>

class TagUpdater : public QThread
{
    Q_OBJECT;

    class QTimer* m_timer;
    class LocalCollection* m_collection;
    QSet<class WsReply*> m_activeRequests;

    // QThread:
    void run();

    int startRequests(QStringList artists);
    void warning(const QString& msg);
    void critical(const QString& msg);

public:
    TagUpdater();
    ~TagUpdater();

private slots:
    void onWsFinished(WsReply*);
    void launchNextBatch();
};

#endif
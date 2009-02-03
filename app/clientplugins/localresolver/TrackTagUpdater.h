/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
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

#ifndef TRACK_TAG_UPDATER_H
#define TRACK_TAG_UPDATER_H

/** @brief Manages track tag requesting: signal needsUpdate as tracks are 
  * added and this class will ensure that requests happen at a controlled
  * rate.  createUpdater creates a thread for the new TrackTagUpdater to 
  * run on.
  *
  */

#include <QObject>
#include <QString>
#include <QDateTime>

class TrackTagUpdater : public QObject
{
    Q_OBJECT

    const QString m_webServiceUrl;
    const unsigned m_tagValidityDays;
    const unsigned m_interRequestDelayMins;

    class QTimer* m_timer;
    class LocalCollection* m_collection;
    bool m_needsUpdate;
    QDateTime m_lastRequestTimeUtc;

    ///

    TrackTagUpdater(const QString& webServiceUrl, unsigned tagValidityDays, unsigned interRequestDelayMins);
    ~TrackTagUpdater()
    {
        // FIXME.  testing only
        int ii = 0;
    }

    void startTimer(int seconds);
    unsigned secondsToNextUpdate();
    int secondsSinceLastRequest();

private slots:
    void doUpdateTags();
    void onFinished(int requestIdCount, int responseIdCount, int responseTagCount);

public:
    static TrackTagUpdater* 
    create(const QString& webServiceUrl, unsigned tagValidityDays, unsigned interRequestDelayMins);

public slots:
    void needsUpdate();

signals:
    void tagsUpdated(int requestIdCount, int responseIdCount, int responseTagCount, int secondsTaken);
};


#endif
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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#ifndef MBID_JOB_H
#define MBID_JOB_H

#include "lib/lastfm/types/Track.h"
#include <QRunnable>
#include <QObject>


class ExtractIdentifiersJob : public QObject, public QRunnable
{
    Q_OBJECT
    
    Track m_gui_track;
    Track m_track;
    QString m_path;

    virtual void run();
    FingerprintId fpid() const;
    static void waitForFinished( class QNetworkReply* );
    
public:
    ExtractIdentifiersJob( const Track& );
    ~ExtractIdentifiersJob();

signals:
    void mbid( const QString& );
    
private slots:
    void onMbid( const QString& mbid );
};

#endif

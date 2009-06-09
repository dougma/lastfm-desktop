/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

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
#ifndef MBID_JOB_H
#define MBID_JOB_H

#include <lastfm/Track>
#include <QRunnable>
#include <QObject>


class ExtractIdentifiersJob : public QObject, public QRunnable
{
    Q_OBJECT
    
    Track m_gui_track;
    Track m_track;
    QString m_path;

    virtual void run();
    FingerprintId fpid();
    
public:
    ExtractIdentifiersJob( const Track& );
    ~ExtractIdentifiersJob();

signals:
    void mbid( const QString& );
    void fingerprinted( const Track& );
    
private slots:
    void onMbid( const QString& mbid );
};

#endif

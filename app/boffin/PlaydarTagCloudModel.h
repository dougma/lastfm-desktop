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
#ifndef PLAYDAR_TAG_CLOUD_MODEL_H
#define PLAYDAR_TAG_CLOUD_MODEL_H

#include "playdar/BoffinTagRequest.h"
#include "playdar/PlaydarApi.h"
#include <lastfm/global.h>
#include <QAbstractItemModel>
#include <QAbstractTableModel>
#include <QSet>
#include <QMap>
#include <QList>
#include <QMultiMap>

class PlaydarConnection;

class PlaydarTagCloudModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum CustomRoles { WeightRole = Qt::UserRole, LinearWeightRole, CountRole, SecondsRole, RelevanceRole };

    PlaydarTagCloudModel(PlaydarConnection *playdar);
    ~PlaydarTagCloudModel(void);

    void startGetTags(const QString& rql = QString());

    void setHostFilter(QSet<QString> hosts);            // exclude hosts from tagcloud
    void addToHostFilter(const QString& hostname);
    void setTagMapping(QMap<QString, QString> tagMap);  // map tagname -> preferred tagname

    int maxTrackCount() const;

    virtual QModelIndex index( int row, int column, const QModelIndex& parent = QModelIndex()) const;
    QModelIndex indexOf( const BoffinTagItem& t );
    virtual QModelIndex parent( const QModelIndex& ) const;
    virtual int rowCount( const QModelIndex& = QModelIndex() ) const;
    virtual int columnCount( const QModelIndex& = QModelIndex() ) const;
    virtual QVariant data( const QModelIndex&, int role = Qt::DisplayRole ) const;
    virtual bool hasChildren( const QModelIndex& = QModelIndex()) const{ return false; }

signals:
	void fetchedTags();
	void tagItem( const BoffinTagItem& );

private slots:
    void onTag(BoffinTagItem tag);
    void onTagError();
    void onFetchedTags();

private:
    PlaydarConnection* m_playdar;

    QSet<QString> m_hostFilter;     // hosts to filter from this tag cloud
    QSet<QString> m_hosts;          // hosts contributing to this tag cloud

    QList< BoffinTagItem > m_tagListBuffer;
    QList< BoffinTagItem > m_tagList;

    BoffinTagItem m_tag;    // the last tag provided via onTags

    int m_maxTrackCount;
    int m_totalTracks;
    int m_totalDuration;

    float m_maxWeight;
    float m_maxLogCount;
    float m_minLogCount;
//    float m_minLogWeight, m_maxLogWeight;

    class QTimer* m_loadingTimer;
};

#endif

/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
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
 
#ifndef PLAYDAR_TAG_CLOUD_MODEL_H
#define PLAYDAR_TAG_CLOUD_MODEL_H

#include <QAbstractItemModel>
#include <QSet>
#include <QMap>
#include <QList>
#include <QMultiMap>
#include "PlaydarApi.h"
#include "BoffinRequest.h"


class PlaydarTagCloudModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum CustomRoles { WeightRole = Qt::UserRole, LinearWeightRole };

    PlaydarTagCloudModel(PlaydarApi& p, class WsAccessManager* wam);
    ~PlaydarTagCloudModel(void);

    void startGetTags();

    void setHostFilter(QSet<QString> hosts);            // exclude hosts from tagcloud
    void setTagMapping(QMap<QString, QString> tagMap);      // map tagname -> preferred tagname

    virtual QModelIndex index( int row, int column, const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex parent( const QModelIndex& ) const;
    virtual int rowCount( const QModelIndex& = QModelIndex() ) const;
    virtual int columnCount( const QModelIndex& =QModelIndex() ) const;
    virtual QVariant data( const QModelIndex&, int role = Qt::DisplayRole ) const;

private slots:
    void onTags(QList<BoffinTagItem> tags);
    void onTagError();

private:
    PlaydarApi m_api;
    WsAccessManager* m_wam;

    QSet<QString> m_hostFilter;
    QMap<QString, QString> m_tagMap;

    QList<BoffinTagItem> m_tags;    // the last taglist provided via onTags

    QMultiMap<float, QString> m_tagHash;
    QMultiMap<float, QString> m_logTagHash;
    float m_maxWeight;
    float m_minLogWeight;
};

#endif

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

#ifndef FIREHOSE_MODEL_H
#define FIREHOSE_MODEL_H

#include <QAbstractItemModel>
#include <QDateTime>
#include <QList>
#include <QPixmap>
#include <QVariant>
#include "lib/lastfm/types/Track.h"


enum
{
    TrackRole = Qt::UserRole,
    TimestampRole,
    CumulativeCountRole
};


class FirehoseModel : public QAbstractItemModel
{
    Q_OBJECT
    
    virtual QVariant data(const QModelIndex &index, int role) const;
    
    virtual QModelIndex index( int row, int column, const QModelIndex& ) const { return createIndex( row, column ); }
    virtual QModelIndex parent(const QModelIndex& ) const { return QModelIndex(); }
    virtual int rowCount( const QModelIndex& ) const { return m_users.count(); }
    virtual int columnCount( const QModelIndex& ) const { return 1; }
    
    QList<QString> m_users;
    QList<QPixmap> m_avatars;
    QList<Track> m_tracks;
    QList<QDateTime> m_timestamps;
    
    class QTcpSocket* m_socket;
    
    /** used to figure out the zebra stripe */
    uint m_cumulative_count;
    
public slots:
    void setNozzle( const QString& );
    void prune(); //prunes the list to 20
    
private slots:
    void onData();
    void onFinished();
    void onItemReady( class FirehoseItem* );
    
public:
    FirehoseModel();
};


class FirehoseItem : public QObject
{
    Q_OBJECT
    
    Track m_track;
    QString const m_user;
    QPixmap m_avatar;
    
public:
    FirehoseItem( const class CoreDomElement& );
    
    Track track() const { return m_track; }
    QString user() const { return m_user; }
    QPixmap avatar() const { return m_avatar; }
    
private slots:
    void onAvatarDownloaded();
    
signals:
    void finished( FirehoseItem* );
};


#endif

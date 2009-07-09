/*
   Copyright 2009 Last.fm Ltd. 
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
#ifndef TAG_BROWSER_WIDGET_H
#define TAG_BROWSER_WIDGET_H

#include <math.h>
#include <float.h>
#include <boost/function.hpp>
#include <QStringList>
#include <QWidget>

class QItemSelection;
class TagCloudView;
class PlaydarTagCloudModel;
class SideBySideLayout;
class HistoryWidget;
class PlaydarConnection;
class PlaylistModel;
class PlaylistWidget;

#include "playdar/PlaydarConnection.h"
#include "PlaydarTagCloudModel.h"
#include <QSortFilterProxyModel>
#include <float.h>
#include <math.h>

class RelevanceFilter : public QSortFilterProxyModel
{
    Q_OBJECT;

public:
    RelevanceFilter()
        :m_req(0)
        ,m_showAll( true )
        ,m_minimumTrackCountFilter( 0 )
        ,m_maxTrackCount( 0 )
    {
    }

    void resetFilter()
    {
        m_logCountMap.clear();
        m_showAll = true;
        invalidateFilter();
    }

    void setRqlFilter(PlaydarConnection* playdar, QString rql, QModelIndexList selected)
    {
        if (rql.length() == 0) {
            resetFilter();
            return;
        }

        m_countMap.clear();
        m_logCountMap.clear();


        m_minTrackCount = FLT_MAX;
        m_maxTrackCount = FLT_MIN;
        
        if (m_req) {
            m_req->disconnect(this);
        }
        m_req = playdar->boffinTagcloud( rql );
        connect(m_req, SIGNAL(tagItem(BoffinTagItem)), SLOT(onTagItem(BoffinTagItem)));

        invalidateFilter();
    }

    void setMinimumTrackCountFilter( int i = 0 )
    {
        m_minimumTrackCountFilter = i;
        invalidateFilter();
    }

    void showRelevant(bool bShowRelevant)
    {
        if (bShowRelevant == m_showAll) {
            m_showAll = !m_showAll;
            invalidateFilter();
        }
    }

    bool showingRelevant()
    {
        return !m_showAll;
    }

    bool showingAll()
    {
        return m_showAll;
    }

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const
    {
        if( m_logCountMap.isEmpty() && sourceModel()->index(source_row, 0, source_parent)
                                    .data( PlaydarTagCloudModel::CountRole ).toInt() < m_minimumTrackCountFilter )
            return false;

        if( !m_logCountMap.isEmpty() &&
            m_logCountMap[sourceModel()->index(source_row, 0, source_parent)
                                    .data().toString()] < m_minimumTrackCountFilter )
            return false;


        if (m_showAll)
            return true;

        QModelIndex i = sourceModel()->index(source_row, 0, source_parent);
        const QString tagname = i.data().toString();
        return m_logCountMap.contains(tagname);
    }

    // we own the RelevanceRole.  :)
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const
    {
        if (role == PlaydarTagCloudModel::RelevanceRole) {
          if( m_logCountMap.isEmpty()) return 0.0;

            const QString tagname = mapToSource(index).data().toString();
            QMap<QString, qreal>::const_iterator i = m_logCountMap.find(tagname);
            
            qreal result;
            
            // A relevance weight will be returned with a value of min < result < max 
            // or 0.0 if there is absolutely no relevance.
            const float min = 0.2f;
            const float max = 0.8f;
            
            //TODO: relevance = 0 where trackCount >= current selected trackCount
            if (i == m_logCountMap.end()) {
                result = 0;
            } else if (m_maxTrackCount == m_minTrackCount) {
                result = max;
            } else {
                const qreal lc = i.value();
                result = min + (( lc - m_minTrackCount ) / ( m_maxTrackCount - m_minTrackCount ) * (max - min));
            }
            return QVariant::fromValue<float>(result);
            
        } else if( role == Qt::ToolTipRole ) {
            int count = 0;
            int duration = 0;

            QModelIndex srcindex( mapToSource(index) );

            //if no tags are selected then display the total track count per tag
            if( m_logCountMap.isEmpty() )
            {
                count = srcindex.data( PlaydarTagCloudModel::CountRole ).toInt();
                duration = srcindex.data( PlaydarTagCloudModel::SecondsRole ).toInt();
            }

            //otherwise calculate the resultant track count based on currently selected tags
            const QString tagname = mapToSource(index).data().toString();
            QMap<QString, int>::const_iterator i = m_countMap.find(tagname);

            if( i != m_countMap.end()) {
                count = i.value();
            }

            return count > 0 ? tr( "%L1 tracks" ).arg( count ) + formatDuration(duration)
                             : tr( "no tracks" );
        }

        return QSortFilterProxyModel::data(index, role);
    }

    static QString formatDuration(int seconds)
    {
        if (seconds == 0)
            return QString();

        int mins = seconds / 60;
        int hours = mins / 60;
        if (hours >= 48) {
            return QString(" : %1 days %2:%3:%4")
                .arg(hours / 24)
                .arg(hours % 24)
                .arg(mins % 60, 2, 10, QChar('0'))
                .arg(seconds % 60, 2, 10, QChar('0'));
        } else if (hours >= 24) {
            return QString(" : 1 day %1:%2:%3")
                .arg(hours % 24)
                .arg(mins % 60, 2, 10, QChar('0'))
                .arg(seconds % 60, 2, 10, QChar('0'));
        } else if (hours >= 1) {
            return QString(" : %1:%2:%3")
                .arg(hours)
                .arg(mins % 60, 2, 10, QChar('0'))
                .arg(seconds % 60, 2, 10, QChar('0'));
        }

        return QString(" : %1:%2")
            .arg(mins)
            .arg(seconds % 60, 2, 10, QChar('0'));
    }

private slots:
    void onTagItem(const BoffinTagItem& tagitem)
    {
        const QString& tag = tagitem.m_name;
        if( tagitem.m_count == 0 )
            return;
        
        if (insertTag(tag, tagitem.m_count)) {
            invalidateFilter();
        }

        // potentially, all our data (ie: the RelevanceRole) has changed
        emit dataChanged(
            this->mapFromSource(sourceModel()->index(0, 0)),
            this->mapFromSource(sourceModel()->index(sourceModel()->rowCount(), 0)));
    }

    // returns true if the tag is new, false if the tag already existed
    bool insertTag(const QString& tag, int count)
    {
        bool result;
        if (m_logCountMap.contains(tag)) {
            m_countMap[tag] += count;
            //log is not a distributive function so need to log total count
            m_logCountMap[tag] = log( (float) m_countMap[tag] );
            result = false;
        } else {
            m_logCountMap[tag] = log( (float) count );
            m_countMap[ tag ] = count;
            result = true;
        }

        m_minTrackCount = qMin( m_minTrackCount, m_logCountMap[ tag ] );
        m_maxTrackCount = qMax( m_maxTrackCount, m_logCountMap[ tag ] );
        
        return result;
    }

private:
    BoffinTagRequest* m_req;
    bool m_showAll;
    int m_minimumTrackCountFilter;
    qreal m_maxTrackCount;
    qreal m_minTrackCount;
    QMap<QString, int> m_countMap;
    QMap<QString, qreal> m_logCountMap;
};


class TagBrowserWidget : public QWidget
{
    Q_OBJECT

public:
    TagBrowserWidget(PlaydarConnection*, QWidget* parent = 0);

    QString human() const;
    QString rql() const;
    QStringList selectedTags() const;

signals:
    void selectionChanged();

private slots:
    void onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void onFilterClicked();
    void onSliderChanged( int );

private:
    class QStringList m_rql;
    class QLabel* m_rqlSentence;

    TagCloudView* m_view;
    class QSlider* m_trackCountSlider;
    RelevanceFilter* m_filter;              // sits between model and view
    PlaydarTagCloudModel* m_tagCloudModel;

    PlaylistWidget* m_playlistWidget;
    PlaylistModel* m_playlistModel;

    QStringList m_tags;
    PlaydarConnection* m_playdar;
};

#endif

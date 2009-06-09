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
        m_map.clear();
        m_countmap.clear();
        m_showAll = true;
        invalidateFilter();
    }

    void setRqlFilter(PlaydarConnection* playdar, QString rql, QModelIndexList selected)
    {
        if (rql.length() == 0) {
            resetFilter();
            return;
        }

	    m_map.clear();
	    m_countmap.clear();
        foreach(const QModelIndex i, selected) {
            m_map.insert(i.data().toString(), i.data(PlaydarTagCloudModel::WeightRole).value<float>());
        }

        m_maxTrackCount = 0;
        m_min = FLT_MAX;
	    m_max = FLT_MIN;
        if (m_req) {
            m_req->disconnect(this);
        }
	    m_req = playdar->boffinTagcloud( rql );
	    connect(m_req, SIGNAL(tagItem(BoffinTagItem)), SLOT(onTagItem(BoffinTagItem)));

        //invalidateFilter();
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
    	if( m_countmap.isEmpty() && sourceModel()->index(source_row, 0, source_parent)
								    .data( PlaydarTagCloudModel::CountRole ).toInt() < m_minimumTrackCountFilter )
    		return false;

    	if( !m_countmap.isEmpty() &&
    		m_countmap[sourceModel()->index(source_row, 0, source_parent)
								    .data().toString()] < m_minimumTrackCountFilter )
    		return false;


        if (m_showAll)
            return true;

        QModelIndex i = sourceModel()->index(source_row, 0, source_parent);
        const QString tagname = i.data().toString();
        return m_map.contains(tagname);
    }

    // we own the RelevanceRole.  :)
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const
    {
        if (role == PlaydarTagCloudModel::RelevanceRole) {
            const QString tagname = mapToSource(index).data().toString();
        	QMap<QString, float>::const_iterator i = m_map.find(tagname);
            float result;
            if (i == m_map.end()) {
        		result = 0;
            } else if (m_min == m_max) {
                result = 1.0;
            } else {
                const float lw = i.value();
                result = (lw - m_min) / (m_max - m_min);
            }
            return QVariant::fromValue<float>(result);
        } else if( role == Qt::ToolTipRole ) {
        	int count = 0;

        	//if no tags are selected then display the total track count per tag
        	if( m_countmap.isEmpty() )
        	{
        		count = mapToSource(index).data( PlaydarTagCloudModel::CountRole ).toInt();
        	}

        	//otherwise calculate the resultant track count based on currently selected tags
            const QString tagname = mapToSource(index).data().toString();
        	QMap<QString, int>::const_iterator i = m_countmap.find(tagname);

        	if( i != m_countmap.end()) {
        		count = i.value();
        	}
        	return count > 0 ? tr( "%1 tracks" ).arg( count )
							 : tr( "no tracks" );
        }

        return QSortFilterProxyModel::data(index, role);
    }

private slots:
    void onTagItem(const BoffinTagItem& tagitem)
    {
        const QString& tag = tagitem.m_name;
        float lw = log(tagitem.m_weight);
        if (insertTag(tag, lw)) {
        	m_countmap[ tag ] = tagitem.m_count;
            invalidateFilter();
        } else {
        	m_countmap[ tag ] += tagitem.m_count;
        }
        m_maxTrackCount = qMax( m_maxTrackCount, m_countmap[ tag ] );
        // potentially, all our data (ie: the RelevanceRole) has changed
        emit dataChanged(
            this->mapFromSource(sourceModel()->index(0, 0)),
            this->mapFromSource(sourceModel()->index(sourceModel()->rowCount(), 0)));
    }

    // returns true if the tag is new, false if the tag already existed
    bool insertTag(const QString& tag, float logweight)
    {
        bool result;
        if (m_map.contains(tag)) {
            logweight = (m_map[tag] += logweight);
            result = false;
        } else {
            m_map[tag] = logweight;
            result = true;
        }

        if (logweight < m_min) {
		    m_min = logweight;
	    }
	    if (logweight > m_max) {
		    m_max = logweight;
	    }
        return result;
    }

private:
    BoffinTagRequest* m_req;
    bool m_showAll;
    int m_minimumTrackCountFilter;
    int m_maxTrackCount;
    QMap<QString, float> m_map;
    QMap<QString, int> m_countmap;
    float m_min, m_max;
};


class TagBrowserWidget : public QWidget
{
    Q_OBJECT

public:
    TagBrowserWidget(PlaydarConnection*, QWidget* parent = 0);

    QString rql();
    QStringList selectedTags() const;

signals:
    void selectionChanged();

private slots:
    void onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void onHistoryClicked(int position, const QString& text);
    void onFilterClicked();
    void onSliderChanged( int );
    void onModelChanged( const QModelIndex&, int, int );

private:
    HistoryWidget* m_history;

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

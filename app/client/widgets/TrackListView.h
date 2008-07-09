/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd                                       *
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

#include <QWidget>
#include "ObservedTrack.h" //FIXME move scrobblePoint


class TrackListView : public QWidget
{
    Q_OBJECT

public:
    TrackListView();

    void add( const class ObservedTrack& );

private slots:
    void onAppEvent( int, const QVariant& );

private:
    struct Ui
    {
        class QBoxLayout* layout;
        class ScrobbleProgressWidget* progress;
    };

    Ui ui;
};


class ScrobbleProgressWidget : public QWidget
{
    Q_OBJECT

public:
    ScrobbleProgressWidget();

    struct Ui
    {
        class QLabel* time;
        class QLabel* timeToScrobblePoint;
    };

    Ui ui;

    /** progress is updated every granularity, so if showing the progress todo
    * scrobble point, pass the scrobble point in seconds, and the granularity
    * will be based on the width of the mainwindow and the scrobble point */
    void determineProgressDisplayGranularity( const class ScrobblePoint& );

    void resizeEvent( QResizeEvent* );
    void paintEvent( QPaintEvent* );

    class QTimer* m_progressDisplayTimer;
    uint m_progressDisplayTick;

    ScrobblePoint m_scrobblePoint;
    ScrobblePoint scrobblePoint() const { return m_scrobblePoint; }

public slots:
    void onPlaybackTick( int );

private slots:
    void onProgressDisplayTick();
};

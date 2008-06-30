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

#include "ui_MainWindow.h"
#include <QMap>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    Ui::MainWindow ui;

protected:
    void resizeEvent( QResizeEvent* );
    void paintEvent( QPaintEvent* );

private slots:
    void onAppEvent( int, const QVariant& );
    void onPlaybackTick( int );
    void onProgressDisplayTick();

public slots:
    void showSettingsDialog();
    void showDiagnosticsDialog();

private:
    /** progress is updated every granularity, so if showing the progress todo
      * scrobble point, pass the scrobble point in seconds, and the granularity
      * will be based on the width of the mainwindow and the scrobble point */
    void determineProgressDisplayGranularity( uint g );

    class QTimer* m_progressDisplayTimer;
    uint m_progressDisplayTick;

    class TrackListView* m_trackListView;
};

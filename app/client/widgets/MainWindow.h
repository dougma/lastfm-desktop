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

#include "ui_MainWindow.h"
#include <QMap>
#include <QSystemTrayIcon> // due to a poor design decision in Qt


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    struct : Ui::MainWindow
    {
        class NowPlayingView* nowPlaying;
        class ScrobbleProgressBar* progress;
    } 
    ui;

protected:
    void closeEvent( QCloseEvent* );

public slots:
    void showSettingsDialog();
    void showDiagnosticsDialog();
    void showAboutDialog();
    void showMetaInfoView();
    
private slots:
    void onSystemTrayIconActivated( QSystemTrayIcon::ActivationReason );
    void onAppEvent( int, const QVariant& );
};

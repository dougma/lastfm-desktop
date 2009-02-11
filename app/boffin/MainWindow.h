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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include <QMainWindow>


class MainWindow : public QMainWindow
{
    Q_OBJECT

    friend class App;

    struct Ui
    {
        class QMenu* account;
        class QMenu* outputdevice;
        class QAction* profile;
        class ScanProgressWidget* progress;
        class ScanLocationsWidget* locations;
    } ui;

public:
    MainWindow();
    
public slots:
    void openProfileUrl();
    
private slots:
    void onUserGotInfo( class WsReply* );
};

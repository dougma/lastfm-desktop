/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Peter Grundstrom and Adam Renburg

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
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "CheckDirTree.h"
#include "ui_mainwindow.h"
#include "lib/unicorn/UnicornMainWindow.h"
#include <QDragEnterEvent>


/** @author <petgru@last.fm> 
  * @author <adam@last.fm>
  * @motherfuckingrewrite <max@last.fm>
  */
class MainWindow : public unicorn::MainWindow
{
    Q_OBJECT
    
signals:
    void startFingerprinting( QStringList dirs );
    void wantsToClose( QCloseEvent* event );
    void logout();
    
public:
    MainWindow();
    
    void dragEnterEvent( QDragEnterEvent* );
    void dropEvent( QDropEvent* );
    
public slots:
    void start();
    
    void showFAQ();
    void aboutDialog();
    
protected:
    void closeEvent( QCloseEvent* );

	Ui::MainWindow ui;

private slots:
    void fingerprintButtonClicked();
};

#endif

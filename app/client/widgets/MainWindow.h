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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "lib/lastfm/types/Track.h"
#include <QSystemTrayIcon> // due to a poor design decision in Qt
#include <QPointer>
#include "State.h"
#include "ui_MainWindow.h"
#include "widgets/UnicornWidget.h"

class ShareDialog;
class TagDialog;
class SettingsDialog;
class DiagnosticsDialog;
class PlaylistDialog;
class LocalRqlDialog;

/** ok it's private, that's insane yeah? Yeah. But this is a global singleton
  * and I had a bug where some stupid insignificant class was manipulating the size
  * of the window! And I spent ages tracing it. So now only a small amount of stuff
  * is public, add what you need, but be conservative! */
class MainWindow : private QMainWindow
{
    Q_OBJECT
    
    friend class App;

public:
    MainWindow();
    ~MainWindow();
    
    using QMainWindow::move;
    using QMainWindow::show;
    using QMainWindow::raise;
    using QMainWindow::setWindowTitle;
    
	struct Ui : ::Ui::MainWindow
	{
		class Amp* amp;
        class SeedsWidget* seeds;
        class TrackDashboardHeader* dashboardHeader;
        class TrackDashboard* dashboard;
        class MessageBar* messagebar;
        
        QAction* localRadio;
        
        class DiagnosticsDialog* diagnostics;
    } ui;

protected:
#ifdef WIN32
    virtual void closeEvent( QCloseEvent* );
#endif
    virtual bool eventFilter( QObject*, QEvent* );

public slots:
    void showSettingsDialog();
    void showAboutDialog();
    void showShareDialog();
	void showTagDialog();
    void showPlaylistDialog();
    void showLocalRqlDialog();

signals:
	void loved();
	void banned();
	
private slots:
    void onSystemTrayIconActivated( QSystemTrayIcon::ActivationReason );
	void onUserGetInfoReturn( class WsReply* );
    void onTrackSpooled( const Track& );
    void onStateChanged( State );
    
private:
    void setupUi();
    
    /** add this widget as a drag handle to move the window */
    void addDragHandleWidget( QWidget* );
	
	virtual void dragEnterEvent( QDragEnterEvent* );
	virtual void dropEvent( QDropEvent* );
    virtual QSize sizeHint() const;
    
    Track m_track;
    
    QMap< QWidget*, QPoint > m_dragHandleMouseDownPos;
	
	UNICORN_UNIQUE_DIALOG_DECL( ShareDialog );
	UNICORN_UNIQUE_DIALOG_DECL( TagDialog );
	UNICORN_UNIQUE_DIALOG_DECL( SettingsDialog );
    UNICORN_UNIQUE_DIALOG_DECL( PlaylistDialog );
    UNICORN_UNIQUE_DIALOG_DECL( LocalRqlDialog );
};

#endif //MAINWINDOW_H

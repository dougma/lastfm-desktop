/*
   Copyright 2005-2009 Last.fm Ltd. 
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
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <lastfm/Track>
#include <QSystemTrayIcon> // due to a poor design decision in Qt
#include <QPointer>
#include "State.h"
#include "ui_MainWindow.h"
#include "lib/unicorn/UnicornMainWindow.h"
class ShareDialog;
class TagDialog;
class SettingsDialog;
class PlaylistDialog;
class LocalRqlDialog;


/** ok it's private, that's insane yeah? Yeah. But this is a global singleton
  * and I had a bug where some stupid insignificant class was manipulating the size
  * of the window! And I spent ages tracing it. So now only a small amount of stuff
  * is public, add what you need, but be conservative! */
class MainWindow : private unicorn::MainWindow
{
    Q_OBJECT

    friend class App;

public:
    MainWindow();

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
        class QAction* localRadio;
        class DiagnosticsDialog* diagnostics;

    	OneDialogPointer<ShareDialog> shareDialog;
    	OneDialogPointer<TagDialog> tagDialog;
    	OneDialogPointer<SettingsDialog> settingsDialog;
        OneDialogPointer<PlaylistDialog> playlistDialog;
        OneDialogPointer<LocalRqlDialog> localRqlDialog;
    } ui;

protected:
#ifdef WIN32
    virtual void closeEvent( QCloseEvent* );
#endif
    virtual bool eventFilter( QObject*, QEvent* );

public slots:
    void showSettingsDialog();
    void showShareDialog();
	void showTagDialog();
    void showPlaylistDialog();
    void showLocalRqlDialog();

signals:
	void loved();
	void banned();

private slots:
    void onSystemTrayIconActivated( QSystemTrayIcon::ActivationReason );
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

    QMap<QWidget*, QPoint> m_dragHandleMouseDownPos;
};

#endif //HEADER_GUARD

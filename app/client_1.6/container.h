/***************************************************************************
 *   Copyright (C) 2005 - 2008 by                                          *
 *      Christian Muehlhaeuser, Last.fm Ltd <chris@last.fm>                *
 *      Erik Jaelevik, Last.fm Ltd <erik@last.fm>                          *
 *      Jono Cole, Last.fm Ltd <jono@last.fm>                              *
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

#ifndef LAST_FM_CONTAINER_H
#define LAST_FM_CONTAINER_H

#include "ui_container.h"
#include "ui_playcontrols.h"
#include <QSystemTrayIcon> //poo :( for enum
class QLabel;


class Container : public QMainWindow
{
    Q_OBJECT

    public:
        Container();
        ~Container();
        
        uint stackIndex() const { return ui.stack->currentIndex(); }
        
        class DiagnosticsDialog* diagnostics;
        class SettingsDialog* settings;

    public slots:
        void onTuningIn( const RadioStation& );

    private:
        struct : Ui::MainWindow
        {
            class ScrobbleLabel* scrobbleLabel;
            class SideBarTree* sidebar;
            class MetaDataWidget* meta;
            class RestStateWidget* rest;

            Ui::PlayControls playcontrols;

        } ui;
        
        void setStopVisible( bool );

    public slots:
//TODO        void checkForUpdates( bool invokedByUser = true );
        void showSettingsDialog();
        void showDiagnosticsDialog();
//TODO        void showShareDialog();
//TODO        void showTagDialog( int defaultTagType = -1 );
//TODO        void showTagDialogMD(); //medical doctor??
        void restoreWindow();
        void toggleWindowVisibility();
        void quit();
//TODO        void scrobbleManualIpod();

        /** tray icon balloon on windows, growl on osx */
        void showNotification( const QString& title, const QString& message );

        void showRestState() { ui.stack->setCurrentIndex( 0 ); }
        void showMetaDataWidget() { ui.stack->setCurrentIndex( 1 ); }

    private slots:
        void onAppStateChanged( State, const Track& );
        void onTrackSpooled( const Track&, StopWatch* );
        void onScrobblePointReached( const Track& );

    signals:
        void becameVisible();
        void play( const QUrl& );
        void skip();
        void stop();

    protected:
        void closeEvent( QCloseEvent* );
        void dropEvent( QDropEvent* );
        void dragEnterEvent( QDragEnterEvent* );
        void dragMoveEvent( QDragMoveEvent* );
        bool event( QEvent* );

        void setupUi();
        void setupTimeBar();
        void applyPlatformSpecificTweaks();
        void applyMenuTweaks();
        void setupConnections();
        void restoreState();

        class TrayIcon* m_trayIcon;

        bool m_userCheck;
        bool m_sidebarEnabled;
        int  m_lastVolume;
        int  m_sidebarWidth;

        #ifndef Q_WS_MAC
        QStyle* m_styleOverrides;
        #endif

    private slots:
        void setupTrayIcon();
        void about();
        void onTrayIconActivated( QSystemTrayIcon::ActivationReason );
        void splitterMoved( int pos ) { m_sidebarWidth = pos; }

        /** opens log in default .log handler */
        void onAltShiftL();
        /** opens MooseUtils::savePath() in default handler */
        void onAltShiftF();

      #ifdef WIN32
        /** opens plugin uninstaller location */
        void onAltShiftP();
      #endif

        void toggleSidebar();
        void toggleScrobbling();
        void toggleDiscoveryMode();

        void onRadioBuffering( int size, int total );

        void displayUrlInStatusBar( const QUrl& url );
        void statusMessage( const QString& message ) { statusBar()->showMessage( message ); }

        void onLoveClicked();
        void onBanClicked();
        void onPlayClicked();
        
        void addToMyPlaylist();
        void volumeUp();
        void volumeDown();
        void mute();

        void showFAQ();
        void showForums();
        void inviteAFriend();

        void updateWindowTitle( const Track& );
        void updateAppearance();

        void minimiseToTray();
        void gotoProfile();

private:
    Track m_track;
    RadioStation m_station;
};


/** The Scrobbling on/off label in the right-hand corner of the statusbar */
class ScrobbleLabel : public QWidget
{
    Q_OBJECT

    QLabel* m_label;
    QLabel* m_image;

public:
    ScrobbleLabel();
    void setEnabled( bool );

    QLabel* label() const { return m_label; }

signals:
    void clicked();

protected:
    virtual void mousePressEvent( QMouseEvent* )
    {
        emit clicked();
    }
};

#endif // CONTAINER_H

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
#include <QSystemTrayIcon> // due to a poor design decision in Qt
#include <QPointer>
#include "PlayerState.h"
#include "lib/types/Track.h"
#include "widgets/UnicornWidget.h"

class ShareDialog;
class TagDialog;
class SettingsDialog;
class DiagnosticsDialog;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    QSize sizeHint() const;
	
	struct Ui : ::Ui::MainWindow
	{
		class Launcher* launcher;
		class PrimaryBucket* primaryBucket;
        class Firehose* firehose;
		class ImageButton* cog;        
        class QLabel* text;
		class PrettyCoverWidget* cover;
    } ui;

protected:
#ifdef WIN32
    virtual void closeEvent( QCloseEvent* );
#endif

public slots:
    void showSettingsDialog();
    void showDiagnosticsDialog();
    void showAboutDialog();
    void showShareDialog();
	void showTagDialog();
    void showMetaInfoView();
    void showCogMenu();
    void closeActiveWindow();

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
    void setupCentralWidget();
	
	virtual void dragEnterEvent( QDragEnterEvent* );
	virtual void dropEvent( QDropEvent* );
    
    Track m_track;
	
	UNICORN_UNIQUE_DIALOG_DECL( ShareDialog );
	UNICORN_UNIQUE_DIALOG_DECL( TagDialog );
	UNICORN_UNIQUE_DIALOG_DECL( SettingsDialog );
	UNICORN_UNIQUE_DIALOG_DECL( DiagnosticsDialog );
};

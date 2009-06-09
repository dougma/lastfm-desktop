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
#include "ui_LoginDialog.h"
#include "lib/DllExportMacro.h"
#include <QDialog>


class UNICORN_DLLEXPORT LoginDialog : public QDialog
{
    Q_OBJECT

public:
    LoginDialog( const QString& username = "" );

    QString passwordHash() const { return m_password; }
    QString username() const { return m_username; }
    QString sessionKey() const { return m_sessionKey; }
    bool isSubscriber() const { return m_subscriber; }
    
private slots:
    void onEdited();
    void authenticate();
    void onAuthenticated();
	void cancel();

private:
    QPushButton* ok() const { return ui.buttonBox->button( QDialogButtonBox::Ok ); }

#ifdef Q_WS_MAC
    struct : Ui::LoginDialog 
	{
		class QDialog* transient;
		class QPushButton* cancel;
		class QProgressBar* progress;
		class QLabel* text;
	}
	ui;
#else
	Ui::LoginDialog ui;
#endif

    QString m_username;
    QString m_password;
    QString m_sessionKey;
    bool m_subscriber;
};

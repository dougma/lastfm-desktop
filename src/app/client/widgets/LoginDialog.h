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

#include "ui_LoginDialog.h"
#include <QDialog>


class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    LoginDialog();

    // is already an md5 hash
    QString password() const { return m_password; }
    QString username() const { return m_username; }

private slots:
    void onEdited();
    void verify();
    void onVerifyResult( class Request* );

private:
    QPushButton* ok() const { return ui.buttonBox->button( QDialogButtonBox::Ok ); }

    Ui::LoginDialog ui;

    bool m_bootstrap;
    QString m_username;
    QString m_password;
};

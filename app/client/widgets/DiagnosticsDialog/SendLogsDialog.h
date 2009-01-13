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

#ifndef SEND_LOGS_DIALOG_H
#define SEND_LOGS_DIALOG_H

#include "ui_SendLogsDialog.h"


/** @author <petgru@openfestis.org>
  * @brief Makes it possible for a user to easily mail all necessary debug information to us.
  */
class SendLogsDialog : public QDialog
{
    Q_OBJECT

public:
    SendLogsDialog( QWidget *parent = 0 );

    Ui::SendLogsDialog ui;

private slots:
    void send();
    void view();
    void onSuccess();
    void onError();
};

#endif

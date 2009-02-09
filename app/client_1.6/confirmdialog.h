/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Christian Muehlhaeuser, Last.fm Ltd <chris@last.fm>                *
 *      Erik Jaelevik, Last.fm Ltd <erik@last.fm>                          *
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

#ifndef CONFIRMDIALOG_H
#define CONFIRMDIALOG_H

#include <QPixmap>
#include <QDialog>
#include "ui_confirmdialog.h"
class TrackInfo;
 

class ConfirmDialog : public QDialog
{
    Q_OBJECT

public:
    ConfirmDialog( QWidget *parent = 0 );
    ~ConfirmDialog();

    /** shows the dialog with exec and returns true if the user clicked the Confirm button */
    static bool share( const TrackInfo&, const QString& username, QWidget* parent );
    static bool tag( const QString&, const QString& username, QWidget* parent );
    static bool playlist( const TrackInfo&, QWidget* parent );
    static bool love( const TrackInfo&, QWidget* parent );
    static bool ban( const TrackInfo&, QWidget* parent );
    static bool quit( QWidget* parent );

    #ifndef Q_WS_X11
    static bool hide( QWidget* parent );
    #endif

    int exec();

    bool isDontAskChecked();

    void setIcon( const QPixmap& );
    void setIcon( const QString& path );
    void setText( const QString& );
    void setConfirmButtonText( const QString&, bool visible = true );
    void setCancelButtonText( const QString&, bool visible = true );
    void setDontAskText( const QString& );
    void setOperationString( const QString& s ) { m_op = s; }

protected:
    virtual void done( int );

private:
    Ui::ConfirmDialog ui;

    QString m_op;
};

#endif // CONFIRMDIALOG_H

/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Max Howell, Last.fm Ltd <max@last.fm>                              *
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

#ifndef REST_STATE_WIDGET_H
#define REST_STATE_WIDGET_H

#include "ui_RestStateWidget.h"
#include "watermarkwidget.h"

class QComboBox;
class QLineEdit;
class QPushButton;


class RestStateWidget : public WatermarkWidget
{
    Q_OBJECT

    Ui::RestStateWidget ui;

public:
    RestStateWidget( QWidget* = 0 );
    ~RestStateWidget();

    void setPlayEnabled( bool b );

public slots:
    void updatePlayerNames();
    void clear();

signals:
    void play( const QUrl& );

private slots:
    void onPlayClicked();
    void onEditTextChanged( const QString& );
    void showBootstrapWizard();
    void openBootstrapFaq();
    void onHandshaken( class Handshake* handshake );

private:
    virtual bool eventFilter( QObject*, QEvent* );

private:
    bool m_play_enabled;
};

#endif

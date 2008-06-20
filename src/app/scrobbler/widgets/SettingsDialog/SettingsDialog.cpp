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

#include "SettingsDialog.h"


SettingsDialog::SettingsDialog()
              : QDialog( qApp->activeWindow() /*FIXME*/ )
{
    ui.setupUi( this );
    // this couldn't be done in designer easily :(
    ui.buttonBox->button( QDialogButtonBox::Ok )->setEnabled( false );
    ui.buttonBox->setContentsMargins( 0, 0, style()->pixelMetric( QStyle::PM_LayoutRightMargin ), 0 );

    // make OK button enable if something changes
    //NOTE we don't store initial value, so as a result if user changes the thing back, we do nothing..
    foreach (QLineEdit* o, ui.pageStack->findChildren<QLineEdit*>())
        connect( o, SIGNAL(textEdited( QString )), SLOT(enableOk()) );
    foreach (QComboBox* o, ui.pageStack->findChildren<QComboBox*>())
        connect( o, SIGNAL(currentIndexChanged( int )), SLOT(enableOk()) );
    foreach (QGroupBox* o, ui.pageStack->findChildren<QGroupBox*>())
        connect( o, SIGNAL(toggled( bool )), SLOT(enableOk()) );
    foreach (QRadioButton* o, ui.pageStack->findChildren<QRadioButton*>())
        connect( o, SIGNAL(toggled( bool )), SLOT(enableOk()) );
}

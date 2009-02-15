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

#include "SettingsDialog.h"
#include "Settings.h"
#include "the/radio.h"
#include "lib/unicorn/UnicornSettings.h"
#include "lib/lastfm/scrobble/private.h"
#include <QtGui>
#include <phonon>

Q_DECLARE_METATYPE( Phonon::AudioOutputDevice );

// Visual Studio sucks, thus we do this
static const unsigned char kChinese[]  = { 0xE4, 0xB8, 0xAD, 0xE6, 0x96, 0x87, 0x0 };
static const unsigned char kRussian[]  = { 0xD0, 0xA0, 0xD1, 0x83, 0xD1, 0x81, 0xD1, 0x81, 0xD0, 0xBA, 0xD0, 0xB8, 0xD0, 0xB9, 0x0 };
static const unsigned char kJapanese[] = { 0xE6, 0x97, 0xA5, 0xE6, 0x9C, 0xAC, 0xE8, 0xAA, 0x9E, 0x0 };


SettingsDialog::SettingsDialog( QWidget* parent )
              : QDialog( parent )
{
    ui.setupUi( this );
    // this couldn't be done in designer easily :(
    ui.buttonBox->button( QDialogButtonBox::Ok )->setEnabled( false );
    ui.buttonBox->setContentsMargins( 0, 0, style()->pixelMetric( QStyle::PM_LayoutRightMargin ), 0 );

    // set the correct start page, as chances are, it got saved wrongly
    ui.pageStack->setCurrentIndex( 0 );

    // Add languages to language drop-down
    ui.languages->addItem( tr( "System Language" ), "" );
    ui.languages->addItem( "English", QLocale::English );
    ui.languages->addItem( QString( "Fran" ) + QChar( 0xe7 ) + QString( "ais" ), QLocale::French );
    ui.languages->addItem( "Italiano", QLocale::Italian );
    ui.languages->addItem( "Deutsch", QLocale::German );
    ui.languages->addItem( QString( "Espa" ) + QChar( 0xf1 ) + QString( "ol" ), QLocale::Spanish );
    ui.languages->addItem( QString( "Portugu" ) + QChar( 0xea ) + QString( "s" ), QLocale::Portuguese );
    ui.languages->addItem( "Polski", QLocale::Polish );
    ui.languages->addItem( "Svenska", QLocale::Swedish );
    ui.languages->addItem( QString::fromUtf8( "Türkçe" ), QLocale::Turkish );
    ui.languages->addItem( QString::fromUtf8( (const char*) kRussian ), QLocale::Russian );
    ui.languages->addItem( QString::fromUtf8( (const char*) kChinese ), QLocale::Chinese );

    ui.logOutOnExit->setChecked( unicorn::Settings().logOutOnExit() );

    moose::Settings s;
    ui.fingerprintingEnabled->setChecked( s.fingerprintingEnabled() );
    ui.iPodScrobblingEnabled->setChecked( s.iPodScrobblingEnabled() );
    ui.alwaysConfirmIPodScrobbles->setChecked( s.alwaysConfirmIPodScrobbles() );
    
    // make OK button enable if something changes
    //NOTE we don't store initial value, so as a result if user changes the thing back, we do nothing..
    foreach (QLineEdit* o, ui.pageStack->findChildren<QLineEdit*>())
        connect( o, SIGNAL(textEdited( QString )), SLOT(enableOk()) );
    foreach (QCheckBox* o, ui.pageStack->findChildren<QCheckBox*>())
        connect( o, SIGNAL(toggled( bool )), SLOT(enableOk()) );
    foreach (QComboBox* o, ui.pageStack->findChildren<QComboBox*>())
        connect( o, SIGNAL(currentIndexChanged( int )), SLOT(enableOk()) );
    foreach (QGroupBox* o, ui.pageStack->findChildren<QGroupBox*>())
        connect( o, SIGNAL(toggled( bool )), SLOT(enableOk()) );
    foreach (QRadioButton* o, ui.pageStack->findChildren<QRadioButton*>())
        connect( o, SIGNAL(toggled( bool )), SLOT(enableOk()) );
    
    foreach (Phonon::AudioOutputDevice d, Phonon::BackendCapabilities::availableAudioOutputDevices())
    {
        ui.outputDevice->addItem( d.name(), QVariant::fromValue( d ) );
        if (d.name() == The::radio().audioOutput()->outputDevice().name())
            ui.outputDevice->setCurrentIndex( ui.outputDevice->count() - 1 );
    }
    
    ui.forbiddenPaths->setPlainText( CoreSettings().value( SCROBBLE_EXCLUSION_DIRS ).toStringList().join( "\n" ) );
}


void //virtual
SettingsDialog::accept()
{
    moose::MutableSettings s;
    CoreSettings cs;

    // note, don't delete the username/password from the settings yet, do that
    // at exit, in case the user changes his/her mind
    s.setLogOutOnExit( ui.logOutOnExit->isChecked() );
    s.setFingerprintingEnabled( ui.fingerprintingEnabled->isChecked() );
    s.setAlwaysConfirmIPodScrobbles( ui.alwaysConfirmIPodScrobbles->isChecked() );

    int const i = ui.languages->currentIndex();
    if (i != -1)
        cs.setValue( "Locale", ui.languages->itemData( i ).toInt() );

    Phonon::AudioOutputDevice d = ui.outputDevice->itemData( ui.outputDevice->currentIndex() ).value<Phonon::AudioOutputDevice>();    
    if (s.audioOutputDeviceName() != d.name())
    {
        s.setAudioOutputDeviceName( d.name() );
        The::radio().audioOutput()->setOutputDevice( d );
    }
    
    cs.setValue( SCROBBLE_EXCLUSION_DIRS, ui.forbiddenPaths->toPlainText().split( '\n' ) );
    
    QDialog::accept();
}

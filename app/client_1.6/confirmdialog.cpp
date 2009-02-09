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

#include "confirmdialog.h"
#include "UglySettings.h"
#include <QPushButton>


ConfirmDialog::ConfirmDialog( QWidget *parent )
        : QDialog( parent, Qt::Sheet )
{
    /// if a drop operation asks for a confirm, cursor is set
    /// to a drag mode, so we have to override it for duration
    qApp->setOverrideCursor( Qt::ArrowCursor );

    ui.setupUi( this );
    ui.line->setFrameShadow( QFrame::Sunken ); // Want etched, not flat
    ui.buttonBox->button( QDialogButtonBox::Ok )->setText( tr( "Confirm" ) );

    //TODO
    setIcon( ":/app_55.png" );

  #ifdef Q_WS_MAC
    ui.line->hide();
    QFont f = font();
    f.setPointSize( 12 );
    setFont( f );
    ui.messageLabel->setFont( f );
    #if QT_VERSION >= 0x00040300
    ui.gridLayout->setVerticalSpacing( 9 );
    #endif
    ui.hboxLayout->setMargin( 16 );
  #endif

    layout()->setSizeConstraint( QLayout::SetFixedSize );

    connect( ui.buttonBox, SIGNAL( accepted() ), SLOT( accept() ) );
    connect( ui.buttonBox, SIGNAL( rejected() ), SLOT( reject() ) );

    adjustSize();
}


ConfirmDialog::~ConfirmDialog()
{
    ///@see ctor   
    qApp->restoreOverrideCursor();
}


void
ConfirmDialog::setIcon( const QPixmap& icon )
{
    ui.iconLabel->setPixmap( icon );
}


void
ConfirmDialog::setIcon( const QString& file )
{
    QPixmap p( file );
    p = p.scaled( 32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
    ui.iconLabel->setPixmap( p );
}


void
ConfirmDialog::setText( const QString& text )
{
    ui.messageLabel->setText( text );
}


void
ConfirmDialog::setConfirmButtonText( const QString& text, bool visible )
{
    ui.buttonBox->button( QDialogButtonBox::Ok )->setText( text );
    ui.buttonBox->button( QDialogButtonBox::Ok )->setVisible( visible );
}

void
ConfirmDialog::setCancelButtonText( const QString& text, bool visible )
{
    ui.buttonBox->button( QDialogButtonBox::Cancel )->setText( text );
    ui.buttonBox->button( QDialogButtonBox::Cancel )->setVisible( visible );
}

void
ConfirmDialog::setDontAskText( const QString& text )
{
    ui.dontAskCheck->setText( text );
}

bool
ConfirmDialog::isDontAskChecked()
{
    return ui.dontAskCheck->checkState() == Qt::Checked;
}


int
ConfirmDialog::exec()
{
    return The::settings().isDontAsk( m_op )
            ? QDialog::Accepted
            : QDialog::exec();
}


void
ConfirmDialog::done( int r )
{
    The::settings().setDontAsk( m_op, isDontAskChecked() );
    QDialog::done( r );
}


bool //static
ConfirmDialog::share( const TrackInfo& track, const QString& username, QWidget* parent )
{
    ConfirmDialog d( parent );
    d.setText( tr( "Are you sure you want to share <b>%1</b> with <b>%2</b>?" ).arg( track.track() ).arg( username ) );
    d.setIcon( ":/action/Share.png" );
    d.setOperationString( "share" );
    d.setConfirmButtonText( tr("Share") );
    return d.exec();
}


bool //static
ConfirmDialog::tag( const QString& itemData, const QString& username, QWidget* parent )
{
    ConfirmDialog d( parent );
    d.setText( tr( "Are you sure you want to tag <b>%1</b> as <b>%2</b>?" ).arg( itemData ).arg( username ) );
    d.setIcon( ":/action/tag.png" );
    d.setOperationString( "tag" );
    d.setConfirmButtonText( tr("Tag") );
    return d.exec();
}


bool //static
ConfirmDialog::playlist( const TrackInfo& track, QWidget* parent )
{
    ConfirmDialog d( parent );
    d.setText( tr( "Are you sure you want to add <b>%1</b> to your playlist?" ).arg( track.track() ) );
    d.setIcon( ":/action/Playlist.png" );
    d.setOperationString( "addToPlaylist" );
    d.setConfirmButtonText( tr("Add") );
    return d.exec();
}


bool //static
ConfirmDialog::love( const TrackInfo& track, QWidget* parent )
{
    ConfirmDialog d( parent );
    d.setText( tr( "Are you sure you want to add <b>%1</b> to your loved tracks?" ).arg( track.track() ) );
    d.setIcon( ":/action/love.png" );
    d.setOperationString( "love" );
    d.setConfirmButtonText( tr("Love") );
    return d.exec();
}


bool //static
ConfirmDialog::ban( const TrackInfo& track, QWidget* parent )
{
    ConfirmDialog d( parent );
    d.setText( tr( "Are you sure you want to ban <b>%1</b>? This means the radio will never play this track again." ).arg( track.track() ) );
    d.setIcon( ":/action/ban.png" );
    d.setOperationString( "ban" );
    d.setConfirmButtonText( tr("Ban") );
    return d.exec();
}


bool //static
ConfirmDialog::quit( QWidget* parent )
{
    ConfirmDialog d( parent );
    d.setText( tr( "Really quit Last.fm? Any music you listen to will not be scrobbled to your profile." ) );
    d.setOperationString( "quit" );
    d.setConfirmButtonText( tr("Quit") );
    return d.exec();
}


#ifndef Q_WS_X11
bool //static
ConfirmDialog::hide( QWidget* parent )
{
    ConfirmDialog d( parent );

    #ifdef WIN32
    d.setText( tr( "The application will keep running in the background. "
                   "You can access it again by launching it from the Start Menu.\n\n"
                   "Use File > Exit to shut down permanently.\n" ) );
    #else
    d.setText( tr( "The application will keep running in the background. "
                   "You can access it again by launching it from the Applications folder.\n\n"
                   "Choose Quit from the application menu to shut down permanently.\n" ) );
    #endif

    d.setOperationString( "hide" );
    d.setConfirmButtonText( tr("OK") );
    d.setCancelButtonText( "", false );
    d.setDontAskText( tr( "Don't tell me again" ) );
    return d.exec();
}
#endif

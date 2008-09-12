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

#include "ShareDialog.h"
#include "Settings.h"
#include "lib/types/User.h"
#include <QLineEdit>
#include <QPainter>
#include <QTimer>
#include <QPushButton>

static const char* kHintText = QT_TR_NOOP( "Type friends' names or emails, comma separated" );


ShareDialog::ShareDialog( QWidget* parent )
        : QDialog( parent )
{
    ui.setupUi( this );
    connect( ui.friends, SIGNAL(editTextChanged( QString )), SLOT(enableDisableOk()) );
    ok()->setText( tr("Share") );
	ok()->setEnabled( false );

    WsReply* r = User( The::settings().username() ).getFriends();
    connect( r, SIGNAL(finished( WsReply* )), SLOT(onFriendsReturn( WsReply* )) );

    ui.friends->lineEdit()->installEventFilter( this );
	
	ui.friends->setFocus();
}


void
ShareDialog::setTrack( const Track& t )
{
    m_track = t;
    ui.artist->setText( tr("The artist, %1").arg( t.artist() ) );
    ui.album->setText( tr("The album, %1").arg( t.album() ) );
    ui.track->setText( tr("The track, %1").arg( t.title() ) );
}


void
ShareDialog::onFriendsReturn( WsReply* r )
{
    QString const edit_text = ui.friends->currentText();
    ui.friends->clear();

	foreach( User user, User::getFriends( r ))
		ui.friends->addItem( user );
    ui.friends->setEditText( edit_text );
}


void
ShareDialog::enableDisableOk()
{
    ok()->setEnabled( ui.friends->currentText().size() );
}


void
ShareDialog::accept()
{
    WsReply* r;

    User recipient( ui.friends->currentText() );
    QString const message = ui.message->toPlainText();

    if (ui.artist->isChecked()) r = m_track.artist().share( recipient, message );
    if (ui.track->isChecked()) r = m_track.share( recipient, message );
    if (ui.album->isChecked()) r = m_track.album().share( recipient, message );

    //TODO feedback on success etc, do via that bar thing you planned

    QDialog::accept();
}


bool
ShareDialog::eventFilter( QObject* o, QEvent* e )
{
    if (e->type() != QEvent::Paint)
        return false;
	
	QLineEdit *w = (QLineEdit*)o;
    if (w->hasFocus() || w->text().size()) 
		return false;
    
    w->event( e );
    
    QString const text = tr( kHintText );
    QPainter p( w );
    p.setPen( Qt::darkGray );	
#ifdef Q_WS_MAC
	QFont f = font();
	f.setPixelSize( 10 );
	p.setFont( f );
    QRect r = w->rect().adjusted( 5, 0, -5, -2 );
#else
	QRect r = w->rect().adjusted( 5, 2, -5, 0 );
#endif
	
    p.drawText( r, Qt::AlignVCenter, text );

    return true; //eat event
}

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
#include "widgets/TrackWidget.h"
#include "lib/types/User.h"
#include <QLineEdit>
#include <QPainter>
#include <QTimer>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QEvent>

static const char* kHintText = QT_TR_NOOP( "Type friends' names or emails, comma separated" );


ShareDialog::ShareDialog( QWidget* parent )
        : QDialog( parent )
{
    setupUi();
    setWindowTitle( tr("Share") );    
    enableDisableOk();
    
    connect( ui.buttons, SIGNAL(accepted()), SLOT(accept()) );
    connect( ui.buttons, SIGNAL(rejected()), SLOT(reject()) );
}


void
ShareDialog::setupUi()
{
    //FIXME duplication in various places, eg TagDialog
    QPalette p = palette();
    p.setBrush( QPalette::Window, QColor( 0x18, 0x18, 0x19 ) );
    p.setBrush( QPalette::WindowText, QColor( 0xff, 0xff, 0xff, 40 ) );
    setPalette( p );
    
    QHBoxLayout* h = new QHBoxLayout;
    h->addWidget( ui.friends = new QLineEdit );
    h->addWidget( new QPushButton( tr("Browse Friends") ) );
    h->setSpacing( 12 );
    
    QVBoxLayout* v1 = new QVBoxLayout;
    v1->addWidget( new QLabel( tr("To") ) );
    v1->addLayout( h );
    v1->setSpacing( 0 );

    QVBoxLayout* v2 = new QVBoxLayout;
    v2->addWidget( new QLabel( tr("Message (optional)") ) );
    v2->addWidget( ui.message = new QTextEdit );
    v2->setSpacing( 4 );
    
    QVBoxLayout* v = new QVBoxLayout( this );
    v->addWidget( ui.track = new TrackWidget );
    v->addLayout( v1 );
    v->addLayout( v2 );
    v->addWidget( ui.buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel ) );
    
    ui.message->setFrameStyle( QFrame::NoFrame );
    
    ui.buttons->button( QDialogButtonBox::Ok )->setText( tr("Share") );
    
#ifdef Q_WS_MAC
    foreach (QLabel* l, findChildren<QLabel*>())
    {
        l->setPalette( p );
        l->setAttribute( Qt::WA_MacSmallSize );
    }
#endif
}


void
ShareDialog::setTrack( const Track& t )
{
    ui.track->setTrack( t );
}


void
ShareDialog::onFriendsReturn( WsReply* r )
{
#if 0
    QString const edit_text = ui.friends->currentText();
    ui.friends->clear();

	foreach( User user, User::getFriends( r ))
		ui.friends->addItem( user );
    ui.friends->setEditText( edit_text );
#endif
}


void
ShareDialog::enableDisableOk()
{
    ok()->setEnabled( ui.friends->text().size() );
}


void
ShareDialog::accept()
{
    User recipient( ui.friends->text() );
    QString const message = ui.message->toPlainText();

    m_track.share( recipient, message );

    //TODO feedback on success etc, do via that bar thing you planned

    QDialog::accept();
}

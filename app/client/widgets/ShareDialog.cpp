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
#include "widgets/FriendsPicker.h"
#include "widgets/TrackWidget.h"
#include "widgets/UnicornWidget.h"
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


ShareDialog::ShareDialog( QWidget* parent )
        : QDialog( parent )
{
    setupUi();
    setWindowTitle( tr("Share") );    
    UnicornWidget::paintItBlack( this );
    enableDisableOk();
    
    connect( ui.edit, SIGNAL(textChanged( QString )), SLOT(enableDisableOk()) );
    connect( ui.browseFriends, SIGNAL(clicked()), SLOT(browseFriends()) );
    
    connect( ui.buttons, SIGNAL(accepted()), SLOT(accept()) );
    connect( ui.buttons, SIGNAL(rejected()), SLOT(reject()) );
}


void
ShareDialog::setupUi()
{
    QHBoxLayout* h = new QHBoxLayout;
    h->addWidget( ui.edit = new QLineEdit );
    h->addWidget( ui.browseFriends = new QPushButton( tr("Browse Friends") ) );
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
    
    ui.message->setAttribute( Qt::WA_MacShowFocusRect, true );
    
    ui.buttons->button( QDialogButtonBox::Ok )->setText( tr("Share") );
    
#ifdef Q_WS_MAC
    foreach (QLabel* l, findChildren<QLabel*>())
        l->setAttribute( Qt::WA_MacSmallSize );
#endif
}


void
ShareDialog::setTrack( const Track& t )
{
    ui.track->setTrack( t );
}


void
ShareDialog::enableDisableOk()
{
    ok()->setEnabled( ui.edit->text().size() );
}


void
ShareDialog::accept()
{
    User recipient( ui.edit->text() );
    QString const message = ui.message->toPlainText();

    m_track.share( recipient, message );

    //TODO feedback on success etc, do via that bar thing you planned

    QDialog::accept();
}


void
ShareDialog::browseFriends()
{
    FriendsPicker fp;
    fp.exec();
    
    foreach (User u, fp.selection())
    {
        qDebug() << u;
    }
}

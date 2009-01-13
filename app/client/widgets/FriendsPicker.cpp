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

#include "FriendsPicker.h"
#include "widgets/HelpTextLineEdit.h"
#include "widgets/UnicornWidget.h"
#include "lib/lastfm/types/User.h"
#include "lib/lastfm/ws/WsReply.h"
#include <QDebug>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QVBoxLayout>


FriendsPicker::FriendsPicker( const User& user )
{    
    qDebug() << user;
    
    QVBoxLayout* v = new QVBoxLayout( this );
    v->addWidget( new HelpTextLineEdit( tr("Search") ) );
    v->addWidget( ui.list = new QListWidget );
    v->addWidget( ui.buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel ) );
 
    UnicornWidget::paintItBlack( this );    
    
    setWindowTitle( tr("Browse Friends") );
    
    WsReply* r = user.getFriends();
    connect( r, SIGNAL(finished( WsReply* )), SLOT(onGetFriendsReturn( WsReply* )) );
    
    connect( ui.buttons, SIGNAL(accepted()), SLOT(accept()) );
    connect( ui.buttons, SIGNAL(rejected()), SLOT(reject()) );
}


void
FriendsPicker::onGetFriendsReturn( WsReply* r )
{
    qDebug() << r;
    
    foreach (User u, User::list( r ))
    {
        ui.list->addItem( u );
    }
}


QList<User>
FriendsPicker::selection() const
{
    return QList<User>();
}
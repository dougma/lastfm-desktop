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
#include "HelpTextLineEdit.h"
#include <lastfm/User>
#include <QDebug>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QVBoxLayout>


FriendsPicker::FriendsPicker( const User& user )
{    
    qDebug() << user;
    
    QVBoxLayout* v = new QVBoxLayout( this );
    v->addWidget( new HelpTextLineEdit( tr("Search your friends") ) );
    v->addWidget( ui.list = new QListWidget );
    v->addWidget( ui.buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel ) );
     
    setWindowTitle( tr("Browse Friends") );
    
    connect( user.getFriends(), SIGNAL(finished()), SLOT(onGetFriendsReturn()) );
    
    connect( ui.buttons, SIGNAL(accepted()), SLOT(accept()) );
    connect( ui.buttons, SIGNAL(rejected()), SLOT(reject()) );
}


void
FriendsPicker::onGetFriendsReturn()
{
    foreach (User u, User::list( (QNetworkReply*)sender() ))
        ui.list->addItem( u );
}


QList<User>
FriendsPicker::selection() const
{
    return QList<User>();
}
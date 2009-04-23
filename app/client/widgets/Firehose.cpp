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

#include "Firehose.h"
#include "FirehoseDelegate.h"
#include "FirehoseModel.h"
#include "widgets/UnicornWidget.h"
#include "widgets/UnicornTabWidget.h"
#include <lastfm/User>
#include <lastfm/WsReply>
#include <QVBoxLayout>


Firehose::Firehose()
{   
    setFrameStyle( QFrame::NoFrame );
    setModel( model = new FirehoseModel );
    setDelegate( new FirehoseDelegate );

    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    
    UnicornWidget::paintItBlack( this );
    QPalette p = palette();
    p.setBrush( QPalette::Base, p.window() );
    p.setBrush( QPalette::Text, p.windowText() );
    setPalette( p );
    setPalette( palette() );
    setAutoFillBackground( true );
    
    connect( AuthenticatedUser().getInfo(), SIGNAL(finished( WsReply* )), SLOT(onUserGotInfo( WsReply* )) );
}


QSize
Firehose::sizeHint() const
{
    return QSize( 358, 600 );
}


void
Firehose::setUserId( uint const id )
{
    qDebug() << id;
    
    ((FirehoseModel*)model)->setNozzle( "user/" + QString::number( id ) + "?rt=xml" );
}


void
Firehose::setStaff()
{
    ((FirehoseModel*)model)->setNozzle( "user/1000002?rt=xml&special=staffmembers" );
}


void
Firehose::onUserGotInfo( WsReply* reply )
{
    qDebug() << reply;
    
    try
    {
        uint const id = reply->lfm()["user"]["id"].text().toInt();
        setUserId( id );
    }
    catch (std::runtime_error&)
    {
        setStaff();
    }
}

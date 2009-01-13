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

#include "WsReplyBlock.h"
#include "WsReply.h"
#include <QEventLoop>
#include <QTimer>


//static 
WsReply* 
WsReplyBlock::wait(WsReply* reply, int timeout)
{
    return WsReplyBlock(reply).waitForFinished(timeout);
}


WsReplyBlock::WsReplyBlock( WsReply* reply )
    : m_reply( reply )
{
    if (reply) {
        connect( reply, SIGNAL(finished( WsReply* )), SLOT(onFinished( WsReply* )) );
    } 
}

WsReply*
WsReplyBlock::waitForFinished( int timeoutMs )
{
    if (m_reply) {
        QTimer::singleShot( timeoutMs, this, SLOT(onFinished()) );
        m_eventloop = new QEventLoop( this );
        m_eventloop->exec();
    }
    return m_reply;
}

void
WsReplyBlock::onFinished( WsReply* reply )
{
    m_reply = reply;
    m_eventloop->exit();
}



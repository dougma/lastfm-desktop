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

#ifndef WS_REPLY_BLOCK_H
#define WS_REPLY_BLOCK_H

#include <lastfm/public.h>
#include <QObject>
class QEventLoop;
class WsReply;

/** @brief Makes it easy to block on a WsReply
  *
  */

class LASTFM_WS_DLLEXPORT WsReplyBlock : public QObject
{
    Q_OBJECT

    WsReply* m_reply;
    QEventLoop* m_eventloop;

    WsReplyBlock( WsReply* reply = 0 );
    WsReply* waitForFinished( int timeout );

public:
    static WsReply* wait(WsReply*, int timeout);

private slots:
    void onFinished( WsReply* reply = 0 );
};

#endif

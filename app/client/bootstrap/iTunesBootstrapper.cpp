/***************************************************************************
*   Copyright (C) 2005 - 2007 by                                          *
*      Jono Cole, Last.fm Ltd <jono@last.fm>                              *
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

#include "iTunesBootstrapper.h"
#include "ITunesDevice/ITunesDevice.h"
#include <QApplication>


iTunesBootstrapper::iTunesBootstrapper( QObject* parent )
                   : AbstractFileBootstrapper( "iTunes", parent )
{
    m_iTunesDatabase = new ITunesDevice;

    connect( m_iTunesDatabase, SIGNAL( progress( int, const Track& ) ),
             this,             SIGNAL( trackProcessed( int, const Track& ) ) );
}


void
iTunesBootstrapper::bootStrap()
{
    Track track;
    if ( m_iTunesDatabase )
    {
        qDebug() << "Reading iTunes database...";
        track = m_iTunesDatabase->firstTrack( m_iTunesDatabase->LibraryPath() );
    }

    while ( !track.isNull() )
    {
        qApp->processEvents();

        if ( !appendTrack( track ) )
        {
            return;
        }

       track =  m_iTunesDatabase->nextTrack();
    }

    zipAndSend();
}


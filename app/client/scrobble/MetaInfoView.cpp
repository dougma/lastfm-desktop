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

#include "MetaInfoView.h"
#include "PlayerEvent.h"
#include "ObservedTrack.h"
#include <QCoreApplication>
#include <QDesktopServices>


MetaInfoView::MetaInfoView( QWidget* parent )
            : QWebView( parent )
{   
   // load( QUrl( "http://www.google.com") );
    page()->setLinkDelegationPolicy( QWebPage::DelegateExternalLinks );
    
    connect( page(), SIGNAL(linkClicked( QUrl )), SLOT(onLinkClicked( QUrl )) );
    connect( qApp, SIGNAL(event( int, QVariant )), SLOT(onAppEvent( int, QVariant )) );
}


MetaInfoView::~MetaInfoView()
{}


void
MetaInfoView::onAppEvent( int e, const QVariant& d )
{
    switch (e)
    {
        case PlayerEvent::TrackChanged:
        case PlayerEvent::PlaybackStarted:
		{
			return; //currently hogs the bandwidth! and slows the main thread too :(
			
            ObservedTrack t = d.value<ObservedTrack>( );
            // FIXME: get the real page.
            load( QUrl( QString( "http://www.last.fm/music/%1" ).arg( t.artist() ) ) );
			qDebug() << QUrl( QString( "http://www.last.fm/music/%1" ).arg( t.artist() ) );
			qDebug() << t.artist();
            break;
		}
		case PlayerEvent::PlaybackEnded:
			//clear the web view
			setHtml( "<html/>" );
    }
}


void
MetaInfoView::onLinkClicked( const QUrl& url )
{
    qDebug() << "Url clicked: " << url;
    QDesktopServices::openUrl( url );
}

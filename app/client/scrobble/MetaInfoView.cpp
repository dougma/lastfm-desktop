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
#include "lib/core/CoreUrl.h"
#include <QAuthenticator>
#include <QCoreApplication>
#include <QDesktopServices>


MetaInfoView::MetaInfoView()
{   
    page()->setLinkDelegationPolicy( QWebPage::DelegateExternalLinks );
    connect( page(), SIGNAL(linkClicked( QUrl )), SLOT(onLinkClicked( QUrl )) );
    connect( qApp, SIGNAL(event( int, QVariant )), SLOT(onAppEvent( int, QVariant )) );
	connect( page()->networkAccessManager(), SIGNAL(authenticationRequired ( QNetworkReply*, QAuthenticator* )), SLOT(onAuthenticationRequired( QNetworkReply*, QAuthenticator* )) );
}


void 
MetaInfoView::onAuthenticationRequired( QNetworkReply*, QAuthenticator* a )
{
	a->setUser( "tester" );
	a->setPassword( "futureofmusic" );
}


void
MetaInfoView::onAppEvent( int e, const QVariant& d )
{
    switch (e)
    {
        case PlayerEvent::TrackChanged:
        case PlayerEvent::PlaybackStarted:
		{
			setHtml( "<html/>" ); //clear the web view
			
			QUrl url = d.value<ObservedTrack>().artist().url();
            load( CoreUrl( url ).mobilised() );
            break;
		}
		case PlayerEvent::PlaybackEnded:
		{
			setHtml( "<html/>" ); //clear the web view
			break;
		}
    }
}


void
MetaInfoView::onLinkClicked( const QUrl& url )
{
    QDesktopServices::openUrl( url );
}

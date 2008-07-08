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
#include "PlaybackEvent.h"

MetaInfoView::MetaInfoView( QWidget* parent ) :
    QWebView( parent )
{
    QWebPage* p1 = page();
    load( QUrl( "http://www.google.com" ) ); 
    QWebPage* p2 = page();
    qDebug() << "WEBPAGES: " << p1 << " : " << p2;
    
    page()->setLinkDelegationPolicy( QWebPage::DelegateExternalLinks );
    
    connect( page(), SIGNAL( linkClicked( const QUrl& ) ),
             this, SLOT( onLinkClicked( const QUrl& ) ) );
}

MetaInfoView::~MetaInfoView()
{

}

void
MetaInfoView::onAppEvent( int e, const QVariant& d )
{
    qDebug() << "MetaInfoView::onAppEvent: " << e << " : " << d;
    switch (e)
    {
        case PlaybackEvent::TrackChanged:
            load( QUrl( "http://www.google.com" ) );
            break;
    }
}

void
MetaInfoView::onLinkClicked( const QUrl& url )
{
    qDebug() << "URL CLICKED: " << url;
    load( QUrl( "http://www.last.fm" ) );
}


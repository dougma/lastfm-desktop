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
#include "lib/core/CoreUrl.h"
#include <QAuthenticator>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QTabBar>
#include <QTemporaryFile>
#include <QVBoxLayout>
#include <QWebSettings>
#include <QWebView>


MetaInfoView::MetaInfoView() 
            : QLabel(tr( "Play some music\nto start scrobbling.") )
{   
	QVBoxLayout* v = new QVBoxLayout( this );
	v->addWidget( ui.web = new QWebView );
	v->setSpacing( 0 );
	v->setMargin( 0 );

    ui.web->hide();
    ui.web->settings()->setUserStyleSheetUrl( QUrl::fromLocalFile( cssPath() ) );
    
	ui.tabs = new QTabBar( ui.web );
    ui.tabs->addTab( tr( "Artist" ) );
	ui.tabs->addTab( tr( "Album" ) );
	ui.tabs->addTab( tr( "Track" ) );
	ui.tabs->setDrawBase( false );
	ui.tabs->hide();
	connect( ui.tabs, SIGNAL(currentChanged( int )), SLOT(load()) );

    connect( qApp, SIGNAL(trackSpooled( Track )), SLOT(onTrackSpooled( Track )) );
    
	ui.web->page()->setLinkDelegationPolicy( QWebPage::DelegateExternalLinks );
    connect( ui.web->page(), SIGNAL(linkClicked( QUrl )), SLOT(onLinkClicked( QUrl )) );
 	connect( ui.web->page()->networkAccessManager(), 
			 SIGNAL(authenticationRequired( QNetworkReply*, QAuthenticator* )), 
			 SLOT(onAuthenticationRequired( QNetworkReply*, QAuthenticator* )) );
	
	setBackgroundRole( QPalette::Base );
    
    QPalette p = palette();
    p.setBrush( QPalette::Base, QColor( 0x18, 0x18, 0x19 ) );
    p.setBrush( QPalette::Text, QColor( 0xff, 0xff, 0xff, 40 ) );
    setPalette( p );

#ifdef Q_WS_MAC
    // large fonts look stupid on Windows
    QFont f = font();
    f.setBold( true );
    f.setPixelSize( 16 ); // indeed pixels are fine on mac and windows, not linux though
    setFont( f );
#endif
    
    
    setAutoFillBackground( true );
    setAlignment( Qt::AlignCenter );
}


void 
MetaInfoView::onAuthenticationRequired( QNetworkReply*, QAuthenticator* a )
{
	a->setUser( "tester" );
	a->setPassword( "futureofmusic" );
}


void
MetaInfoView::onTrackSpooled( const Track& t )
{
    if (t.isNull())
    {
        ui.tabs->hide();
        ui.web->hide();
        ui.web->load( QUrl("about:blank") ); //clear the web view
    }
    else {
        //ui.tabs->show();
        ui.web->show();
        m_track = t;
        load();
    }
}


void
MetaInfoView::onLinkClicked( const QUrl& url )
{
    QDesktopServices::openUrl( url );
}


void
MetaInfoView::resizeEvent( QResizeEvent* )
{
	ui.tabs->move( ui.web->width() - 24 - ui.tabs->sizeHint().width(), 12 );
}


void
MetaInfoView::load()
{
	ui.web->load( QUrl("about:blank") ); //clear the web view first
	
	QUrl url;
	switch (ui.tabs->currentIndex())
	{
		case 0: url = m_track.artist().www(); break;
		case 1: url = m_track.album().www(); break;
		case 2: url = m_track.www(); break;
	}

	ui.web->load( CoreUrl( url ).mobilised() );
    qDebug() << url;
}


QString
MetaInfoView::cssPath()
{
    QTemporaryFile* tmp = new QTemporaryFile( this );
    tmp->open();
    
    QFile f( ":/black.css" );
    f.open( QFile::ReadOnly );
    QTextStream( tmp ) << f.readAll();
    
    return tmp->fileName();
}

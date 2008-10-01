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
#include "lib/ws/WsAccessManager.h"
#include <QAuthenticator>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QTemporaryFile>
#include <QVBoxLayout>
#include "widgets/UnicornTabWidget.h"
#include "widgets/TagListWidget.h"
#include "widgets/SimilarArtists.h"

Bio::Bio(QWidget *parent)
: QWebView(parent)
{
	settings()->setUserStyleSheetUrl( QUrl::fromLocalFile( cssPath() ) );
	page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
	connect(this, SIGNAL(linkClicked(const QUrl &)), SLOT(onLinkClicked(const QUrl &)) );
}


void 
Bio::setContent(CoreDomElement &lfm)
{
#define ARTIST_CLASS "artist"
#define PLAYS_CLASS "plays"
#define CONTENT_CLASS "content"

	try {
		CoreDomElement a = lfm["artist"];
		QString name = a["name"].text();
		QString	plays = a["stats"]["playcount"].text();
		QString listeners = a["stats"]["listeners"].text();
		QString content = a["bio"]["content"].text();

		QStringList html;
		html <<	
			"<p class=\""ARTIST_CLASS"\">" << name << "</p>" <<
			"<p class=\""PLAYS_CLASS"\">" << tr("%L1 plays (%L2 listeners)").arg(plays.toInt()).arg(listeners.toInt()) << "</p>" <<
			"<p class=\""CONTENT_CLASS"\">" << content << "</p>";

		setHtml(html.join(""));
	}
	catch (...)
	{
		qDebug() << "unhandled exception processing artist.getInfo response";
	}
}

void 
Bio::onLinkClicked(const QUrl &url)
{
	QDesktopServices::openUrl(url);
}


QString
Bio::cssPath()
{
	QTemporaryFile* tmp = new QTemporaryFile( this );
	tmp->open();

	QFile f( ":/black.css" );
	f.open( QFile::ReadOnly );
	QTextStream( tmp ) << f.readAll();

	return tmp->fileName();
}



///////////////////

MetaInfoView::MetaInfoView() 
            : QLabel(tr( "Play some music\nto start scrobbling.") )
{   
	QVBoxLayout* v = new QVBoxLayout( this );
	v->setSpacing( 0 );
	v->setMargin( 0 );
	v->addWidget(ui.infoTabs = new Unicorn::TabWidget);
	ui.infoTabs->addTab( tr("Bio"), ui.bio = new Bio(ui.infoTabs) );
	ui.infoTabs->addTab( tr("Tags"), ui.artistTags = new TagIconView );
	ui.infoTabs->addTab( tr("Similar Artists"), ui.similar = new SimilarArtists() );

	//v->addWidget( ui.web = new QWebView );

    ui.infoTabs->hide();
    //ui.bio->
    
	//ui.tabs = new QTabBar( ui.web );
 //   ui.tabs->addTab( tr( "Artist" ) );
	//ui.tabs->addTab( tr( "Album" ) );
	//ui.tabs->addTab( tr( "Track" ) );
	//ui.tabs->setDrawBase( false );
	//ui.tabs->hide();
	//connect( ui.tabs, SIGNAL(currentChanged( int )), SLOT(load()) );

    connect( qApp, SIGNAL(trackSpooled( Track )), SLOT(onTrackSpooled( Track )) );
    connect( qApp, SIGNAL(stateChanged( State )), SLOT(onStateChanged( State )) );
    
	//ui.web->page()->setLinkDelegationPolicy( QWebPage::DelegateExternalLinks );
 //   connect( ui.web->page(), SIGNAL(linkClicked( QUrl )), SLOT(onLinkClicked( QUrl )) );
 //	connect( ui.web->page()->networkAccessManager(), 
	//		 SIGNAL(authenticationRequired( QNetworkReply*, QAuthenticator* )), 
	//		 SLOT(onAuthenticationRequired( QNetworkReply*, QAuthenticator* )) );
	
	setBackgroundRole( QPalette::Base );
    
    QPalette p = palette();
    p.setBrush( QPalette::Base, QColor( 35, 35, 35 ) );
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
	if (t.artist() != m_track.artist())
	{
		ui.similar->clear();
		connect(t.artist().getInfo(), SIGNAL(finished(WsReply*)), this, SLOT(onArtistInfo(WsReply*)) );
		connect(t.artist().getSimilar(), SIGNAL(finished(WsReply*)), this, SLOT(onSimilar(WsReply*)) );
	}
    m_track = t;
    load();
}

void 
MetaInfoView::onArtistInfo(WsReply *reply)
{
	CoreDomElement r = reply->lfm();
	ui.bio->setContent(r);
}

void 
MetaInfoView::onSimilar(WsReply *reply)
{
	ui.similar->setContent(reply->lfm());
}


void
MetaInfoView::onStateChanged( State state )
{
    switch (state)
    {
        case Stopped:
            ui.infoTabs->hide();
            break;
            
        case TuningIn:
            ui.infoTabs->show();
            //ui.web->load( QUrl("about:blank") );
            break;
            
        default:
            ui.infoTabs->show();
            break;
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
	//ui.tabs->move( ui.web->width() - 24 - ui.tabs->sizeHint().width(), 12 );
}


void
MetaInfoView::load()
{   
    if (m_track.isNull())
        return;

    ui.infoTabs->show();
	//ui.web->load( QUrl("about:blank") ); //clear the web view first
	
	//QUrl url;
	//switch (ui.tabs->currentIndex())
	//{
	//	case 0: url = m_track.artist().www(); break;
	//	case 1: url = m_track.album().www(); break;
	//	case 2: url = m_track.www(); break;
	//}

	//ui.web->load( CoreUrl( url ).mobilised() );
//    qDebug() << url;
}




QSize
MetaInfoView::sizeHint() const
{
    return QSize( QLabel::sizeHint().width(), 330 );
}

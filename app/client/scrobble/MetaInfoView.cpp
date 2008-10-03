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

void Bio::clearContent()
{
	setHtml("");
}

void 
Bio::setContent(CoreDomElement &lfm)
{
#define ARTIST_CLASS "artist"
#define PLAYS_CLASS "plays"
#define CONTENT_CLASS "content"
#define EDITME_CLASS "editme"

	try {
		CoreDomElement a = lfm["artist"];
		QString name = a["name"].text();
		QString url = a["url"].text();
		QString	plays = a["stats"]["playcount"].text();
		QString listeners = a["stats"]["listeners"].text();
		QString content = a["bio"]["content"].text();
		QString editmessage = tr("edit this online");

		QString html;
		QTextStream(&html) <<
			"<a class=\""ARTIST_CLASS"\" href=\"" << url << "\">" << name << "</a>" <<
			"<p class=\""PLAYS_CLASS"\">" << tr("%L1 plays (%L2 listeners)").arg(plays.toInt()).arg(listeners.toInt()) << "</p>" <<
			"<p class=\""CONTENT_CLASS"\">" << content.replace("\r", "<br>") << "</p>" <<
			"<a class=\""EDITME_CLASS"\" href=\"" << url << "/+wiki/edit" << "\"><img src= />" << editmessage << "</a>";
		setHtml(html);
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
	ui.infoTabs->addTab( tr("Tags"), ui.trackTags = new TagListWidget );
	ui.infoTabs->addTab( tr("Similar Artists"), ui.similar = new SimilarArtists );
    
    ui.infoTabs->hide();
    ui.infoTabs->bar()->succombToTheDarkSide();

    connect( qApp, SIGNAL(trackSpooled( Track )), SLOT(onTrackSpooled( Track )) );
    connect( qApp, SIGNAL(stateChanged( State )), SLOT(onStateChanged( State )) );
	
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
MetaInfoView::onTrackSpooled( const Track& t )
{
	if (t.artist() != m_track.artist())
	{
		ui.bio->clearContent();
		ui.similar->clear();
		connect( m_artistInfoReply = t.artist().getInfo(), SIGNAL(finished(WsReply*)), SLOT(onArtistInfo(WsReply*)) );
		connect( m_artistSimilarReply = t.artist().getSimilar(), SIGNAL(finished(WsReply*)), SLOT(onSimilar(WsReply*)) );

		ui.trackTags->setTagsRequest( t.getTopTags() );
	}
    m_track = t;
}


void 
MetaInfoView::onArtistInfo(WsReply *reply)
{
	if (m_artistInfoReply == reply) {
		ui.bio->setContent(reply->lfm());
	}
}


void 
MetaInfoView::onSimilar(WsReply *reply)
{
	if (m_artistSimilarReply == reply) {
		ui.similar->setContent(reply->lfm());
	}
}


void
MetaInfoView::onStateChanged( State state )
{
    switch (state)
    {
        case Stopped:
            ui.infoTabs->hide();
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


QSize
MetaInfoView::sizeHint() const
{
    return QSize( QLabel::sizeHint().width(), 330 );
}

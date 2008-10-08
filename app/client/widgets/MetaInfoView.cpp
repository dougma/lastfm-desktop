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
#include <QCoreApplication>
#include <QDesktopServices>
#include <QTemporaryFile>
#include <QVBoxLayout>
#include "widgets/SimilarArtists.h"
#include "widgets/TagListWidget.h"
#include "widgets/UnicornTabWidget.h"
#include "widgets/UnicornWidget.h"
#include "lib/lastfm/core/CoreUrl.h"
#include "lib/lastfm/core/CoreDomElement.h"
#include "lib/lastfm/ws/WsAccessManager.h"
#include "lib/lastfm/ws/WsReply.h"


Bio::Bio()
{
	settings()->setUserStyleSheetUrl( QUrl::fromLocalFile( cssPath() ) );
	page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
	connect(this, SIGNAL(linkClicked(const QUrl &)), SLOT(onLinkClicked(const QUrl &)) );
}


void
Bio::clearContent()
{
	setHtml("");
}


void 
Bio::setContent( const CoreDomElement &lfm )
{
#define ARTIST_CLASS "artist"
#define PLAYS_CLASS "plays"
#define CONTENT_CLASS "content"
#define EDITME_CLASS "editme"

	try 
    {
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
	catch (CoreDomElement::Exception& e)
	{
		qWarning() << e;
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
            : QLabel( tr("<b style='font-size:16pt'>Play some music<br>to start scrobbling.") )
			, m_artistInfoReply( 0 )
			, m_artistSimilarReply( 0 )
{   
	QVBoxLayout* v = new QVBoxLayout( this );
	v->setSpacing( 0 );
	v->setMargin( 0 );
	v->addWidget( ui.tabs = new Unicorn::TabWidget );
	ui.tabs->addTab( tr("Bio"), ui.bio = new Bio );
	ui.tabs->addTab( tr("Tags"), ui.tags = new TagListWidget );
	ui.tabs->addTab( tr("Similar Artists"), ui.similar = new SimilarArtists );
    
    connect( qApp, SIGNAL(trackSpooled( Track )), SLOT(onTrackSpooled( Track )) );
    connect( qApp, SIGNAL(stateChanged( State )), SLOT(onStateChanged( State )) );
	    
    UnicornWidget::paintItBlack( this );
    UnicornWidget::paintItBlack( ui.tags ); // you have to explicitly set
    UnicornWidget::paintItBlack( ui.similar ); // item views, or they stay white

    ui.tabs->hide();
    ui.tabs->bar()->succombToTheDarkSide();

    setAutoFillBackground( true );
    setAlignment( Qt::AlignCenter );
}


void
MetaInfoView::onTrackSpooled( const Track& t )
{
    if (t.isNull()) // null track means playback ended
        return;
    
	if (t.artist() != m_track.artist())
	{
		// clear the ui and cancel the previous requests.
		ui.bio->clearContent();
		ui.similar->clear();
		delete m_artistInfoReply;
		delete m_artistSimilarReply;
		// issue the new requests and connect them up
		connect( m_artistInfoReply = t.artist().getInfo(), SIGNAL(finished(WsReply*)), SLOT(onArtistInfo(WsReply*)) );
		connect( m_artistSimilarReply = t.artist().getSimilar(), SIGNAL(finished(WsReply*)), SLOT(onSimilar(WsReply*)) );

		// TagListWidget internalises the pattern:
		ui.tags->setTagsRequest( t.getTopTags() );
	}
    m_track = t;
}


void 
MetaInfoView::onArtistInfo(WsReply *reply)
{
	ui.bio->setContent(reply->lfm());
	m_artistInfoReply = 0;
}


void 
MetaInfoView::onSimilar(WsReply *reply)
{
	ui.similar->setContent(reply->lfm());
	m_artistSimilarReply = 0;
}


void
MetaInfoView::onStateChanged( State state )
{
    switch (state)
    {
        case Stopped:
            ui.tabs->hide();
            break;
            
        default:
            ui.tabs->show();
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

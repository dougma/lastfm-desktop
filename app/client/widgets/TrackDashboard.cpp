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

#include "TrackDashboard.h"
#include "FadingScrollbar.h"
#include "MainWindow/MediaPlayerIndicator.h"
#include "MainWindow/PrettyCoverWidget.h"
#include "UnicornWidget.h"
#include "lib/unicorn/TrackImageFetcher.h"
#include "lib/unicorn/widgets/SpinnerLabel.h"
#include "lib/lastfm/types/Tag.h"
#include "lib/lastfm/ws/WsReply.h"
#include "lib/lastfm/ws/WsAccessManager.h"
#include "radio/buckets/SeedTypes.h"
#include "PlayableMimeData.h"
#include <QtGui>
#include <QSvgRenderer>
#include <QtWebKit>

#define HEADING "<div style='color:white;font-size:large'><b>"


struct ListView : QListWidget
{
    ListView() : m_sType( Seed::Undefined )
    {
        UnicornWidget::paintItBlack( this );
        
        QPalette p = palette();
        p.setColor( QPalette::Text, QColor( 0xb0b0b0 ) );
        setPalette( p );
        
        setAlternatingRowColors( true );
        setAttribute( Qt::WA_MacShowFocusRect, false );
        
        setDragEnabled( true );
    }
    
    QMimeData* mimeData( const QList<QListWidgetItem*> l ) const
    {
        if( m_sType < 0 || l.isEmpty() )
            return 0;
        
        QListWidgetItem* item = l.first();
        
        switch ( m_sType ) 
        {
            case Seed::ArtistType: return PlayableMimeData::createFromArtist( item->text());
            case Seed::TagType: return PlayableMimeData::createFromTag( item->text());
            default: return NULL;
        }
    }
    
    void setSeedType( Seed::Type t ){ m_sType = t; }
    
private:
    Seed::Type m_sType;
};




TrackDashboard::TrackDashboard()
{   
    nam = new WsAccessManager( this );
    
    ui.papyrus = new QWidget( this );    
    ui.cover = new PrettyCoverWidget;
    ui.cover->setParent( ui.papyrus );
    
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    
    ui.info = new QWidget( ui.papyrus );
    QVBoxLayout* v = new QVBoxLayout( ui.info );
    
    v->addWidget( ui.bio = new QWebView );
    v->addSpacing( 10 );
    v->addWidget( new QLabel( tr( HEADING "Tags") ) );
    v->addSpacing( 3 );
    v->addWidget( ui.tags = new ListView );
    ((ListView*)ui.tags)->setSeedType( Seed::TagType );
    v->addSpacing( 10 );
    v->addWidget( new QLabel( tr( HEADING "Similar Artists") ) );
    v->addSpacing( 3 );
    v->addWidget( ui.similarArtists = new ListView );
    ((ListView*)ui.similarArtists)->setSeedType( Seed::ArtistType );
    v->setMargin( 0 );
    v->setSpacing( 0 );
    
    ui.bio->page()->mainFrame()->setScrollBarPolicy( Qt::Vertical, Qt::ScrollBarAlwaysOff );
    ui.bio->page()->mainFrame()->setScrollBarPolicy( Qt::Horizontal, Qt::ScrollBarAlwaysOff );
    ui.bio->page()->setLinkDelegationPolicy( QWebPage::DelegateExternalLinks );
    connect( ui.bio, SIGNAL(linkClicked( const QUrl& )), SLOT(openExternally( const QUrl& )) );

    ui.scrollbar = new FadingScrollBar( this );
    ui.scrollbar->setVisible( false );
    connect( ui.scrollbar, SIGNAL(valueChanged( int )), SLOT(setPapyrusPosition( int )) );
        
#ifndef Q_WS_MAC
    // mac has finer grained scrolling, and acceleration
    ui.scrollbar->setSingleStep( 9 );
#endif
    
    UnicornWidget::paintItBlack( this );
    UnicornWidget::paintItBlack( ui.bio );
    setAutoFillBackground( true );
    
    QPalette p = palette();
    QColor c( 0x0e, 0x0e, 0x0e );
    p.setColor( QPalette::Window, c );
    p.setColor( QPalette::Base, Qt::transparent );
    p.setColor( QPalette::Text, Qt::white );
    setPalette( p );
    p.setColor( QPalette::Text, QColor( 0xa3, 0xa5, 0xa8 ) );
    ui.bio->setPalette( p );
    
	QFile file( ":/black.css" );
	file.open( QFile::ReadOnly );
    QString css = file.readAll();

    QFont font = this->font();
    font.setPointSize( 11 );

	ui.spinner = new SpinnerLabel( this );
    ui.spinner->move( 10, 10 );
    
    clear();
}


void
TrackDashboard::setTrack( const Track& t )
{    
    if (m_track.artist() != t.artist())
    {
        ui.info->hide();

        qDeleteAll( findChildren<WsReply*>() );
        
        WsReply* r;
        r = t.artist().getInfo();
        r->setParent( this );
        connect( r, SIGNAL(finished( WsReply* )), SLOT(onArtistGotInfo( WsReply* )) );
        
        ui.spinner->show();
        
        r = t.artist().getTopTags();
        connect( r, SIGNAL(finished( WsReply* )), SLOT(onArtistGotTopTags( WsReply* )) );
        r->setParent( this );
    }

    if (m_track.album() != t.album())
    {
        ui.cover->clear();
        
        qDeleteAll( findChildren<TrackImageFetcher*>() );

        TrackImageFetcher* fetch = new TrackImageFetcher( t, nam );
        fetch->setParent( this );
        connect( fetch, SIGNAL(finished( QImage )), ui.cover, SLOT(setImage( QImage )) );
        connect( fetch, SIGNAL(finished( QImage )), fetch, SLOT(deleteLater()) );
        fetch->start();
    }

    m_track = t;

    resizeEvent( 0 );
}


void
TrackDashboard::tuningIn()
{
    clear();
    ui.spinner->show();
}


void
TrackDashboard::clear()
{
    ui.cover->clear();
    ui.bio->setUrl( QUrl("about:blank") );
    ui.scrollbar->setRange( 0, 0 );
	ui.spinner->hide();
    ui.info->hide();    
    ui.scrollbar->hide();
    
    qDeleteAll( findChildren<WsReply*>() );
    qDeleteAll( findChildren<TrackImageFetcher*>() );
    
    m_track = Track();
    
    update();
}


void
TrackDashboard::onArtistGotInfo( WsReply* reply )
{    
    ui.spinner->hide();
    
	try
    {
		CoreDomElement e = reply->lfm()["artist"];
		QString name = e["name"].text();
		QString url = e["url"].text();
		uint plays = e["stats"]["playcount"].text().toUInt();
		uint listeners = e["stats"]["listeners"].text().toUInt();
		QString content = e["bio"]["content"].text();
		QString editmessage = tr("Edit it too!");

		QString html;        
        if (content.isEmpty())
        {
            // this should be all one tr, but meh.
            html = "<p>" + tr("We don't have a description for this artist yet.") + "<p><a href='" + 
                   url + "/+wiki/edit'>" + tr("Why not write one?") + "</a>";
        }
        else
            QTextStream( &html ) <<
                    "<h1>" << name << "</h1>"
                    "<p id=stats>" << tr( "%L1 listeners" ).arg( listeners ) << "<br>"
                                   << tr( "%L1 plays" ).arg( plays ) <<
                    "<p id=content>" << content.replace(QRegExp("\r+"), "<p>") <<
                    "<p id=editme style='margin-top:0'>" << tr("This information was created by users like you! ") <<
                    "<a href=\"" << url << "/+wiki/edit" << "\">" << editmessage << "</a>";
        
        
        QString css = 
            "<style>"
                "body{padding:0;margin:0;color:#bbb}"
                "#stats{color:#444444;margin:0;line-height:1.3;font-weight:bold}"
                "p{line-height:1.6em}"
                "h1{color:#fff;margin:0 0 2px 0}"
                "a{color:#00aeef;text-decoration:none}"
                "a:hover{text-decoration:underline}"
            #ifdef Q_WS_MAC
                "body{font-family:Lucida Grande;font-size:11px}"
                "h1{font-size:18px}"
            #endif
            "</style>";
        
        ui.bio->setHtml( css + html );
        resizeEvent( 0 );
        
        ui.similarArtists->clear();
        foreach (CoreDomElement artist, e["similar"].children( "artist" ))
            ui.similarArtists->addItem( artist["name"].text() );
        
        ui.info->show();
        
        m_artist_image_url = e["image size=large"].text();
        
//        ui.cover->setMinimumWidth( ui.artist->height() );
	}
	catch (CoreDomElement::Exception& e)
	{
		qWarning() << e;
	}
}


void
TrackDashboard::onArtistGotTopTags( WsReply* reply )
{
    ui.tags->clear();
    WeightedStringList tags = Tag::list( reply );
    for (int x = 0, n = qMin( tags.size(), 8 ); x < n; ++x)
        ui.tags->addItem( tags[x] );
}


void
TrackDashboard::resizeEvent( QResizeEvent* )
{    
    if (m_track.isNull())
        return;
    
    ui.papyrus->setFixedWidth( width() );    

    int w = 0;
    
    if (width() > ui.cover->widthForHeight( height() ) * 2)
    {
        ui.scrollbar->fadeIn();
        ui.cover->setParent( this );
        ui.cover->show();
        ui.cover->raise();
        ui.cover->move( 15, 12 );

        int h = height() - 12;
        ui.cover->resize( ui.cover->widthForHeight( h ), h );
        
        ui.info->move( ui.cover->geometry().right() + 16, 0 );
        
        w = width() - ui.info->geometry().x() - ui.scrollbar->sizeHint().width() - 12;
    }
    else
    {
        ui.scrollbar->fadeOutLater();
        ui.cover->setParent( ui.papyrus );
        ui.cover->move( 0, 0 );
        ui.cover->show();
        ui.cover->raise();
        
        w = qMax( ui.cover->widthForHeight( height() - 12 ), 180 );
        int const x = (width() - w) / 2;

        ui.cover->resize( ui.papyrus->width(), height() - 12 );
        ui.info->move( x, ui.cover->height() + 12 );
    }

    ui.info->setFixedWidth( w );
    ui.bio->page()->setViewportSize( QSize(w, height()) );
    const int h = ui.bio->page()->mainFrame()->contentsSize().height();
    ui.bio->setFixedSize( w, h );
    ui.info->adjustSize();

    ui.papyrus->resize( 0xff, ui.info->height() + ui.info->geometry().y() );

    {
        const int w = ui.scrollbar->sizeHint().width();
        ui.scrollbar->setGeometry( width() - w, 0, w, height() );
    }
 
    if (!m_track.isNull())
    {
        ui.scrollbar->setRange( 0, ui.papyrus->height() - height() );
        ui.scrollbar->setPageStep( height() );
    }
    
    setPapyrusPosition( ui.scrollbar->value() );
    ui.scrollbar->raise();
    
    if (m_track.isNull())
        ui.scrollbar->hide();
}


void
TrackDashboard::setPapyrusPosition( int y )
{
    int const fudge = orientation() == Qt::Horizontal ? 10 : 15;    
    ui.papyrus->move( 0, fudge - y );
}


bool
TrackDashboard::event( QEvent* e )
{
    switch ((int)e->type())
    {
        case QEvent::Enter:
            if (ui.scrollbar->maximum() > 0)
                ui.scrollbar->fadeIn();
            break;
        
        case QEvent::FocusOut:
        case QEvent::Leave:
            if (orientation() != Qt::Horizontal)
                ui.scrollbar->fadeOut();
            break;
            
        case QEvent::Wheel:
            ui.scrollbar->event( e );
            break;
    }
    
    return QWidget::event( e );
}


void
TrackDashboard::paintEvent( QPaintEvent* e )
{
    QPainter p( this );
    p.setPen( QColor( 0x161616 ) );
    p.drawLine( 0, 0, width(), 0 );
    p.setPen( QColor( 0x101010 ) );    
    p.drawLine( 0, 1, width(), 1 );
    
    if (!m_track.isNull())
        return;
    
    p.setClipRect( e->rect() );
    p.setRenderHint( QPainter::Antialiasing );
    p.setRenderHint( QPainter::SmoothPixmapTransform );
    
    QSvgRenderer svg( QString(":/lastfm/as.svg") );
    
    QSize s = svg.defaultSize() * 5;
    s.scale( 120, 0, Qt::KeepAspectRatioByExpanding );
    QRect r = QRect( rect().center() - QRect( QPoint(), s ).center(), s );
    
    p.setOpacity( qreal(40)/255 );
    svg.render( &p, r );    
}


void
TrackDashboard::openExternally( const QUrl& url )
{
    QDesktopServices::openUrl( url );
}

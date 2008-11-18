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
#include "lib/unicorn/widgets/SpinnerLabel.h"
#include "lib/lastfm/types/Artist.h"
#include "lib/lastfm/types/Tag.h"
#include "lib/lastfm/ws/WsReply.h"
#include "lib/lastfm/ws/WsAccessManager.h"
#include <QtGui>
#include <QSvgRenderer>

#define HEADING "<div style='color:white;font-size:large'><b>"


namespace mxcl
{
    struct TextBrowser : QTextBrowser
    {
        TextBrowser()
        {
            setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
            setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        }
        
        virtual QSize sizeHint() const
        {
            return QSize( width(), document()->size().height() );
        }
    };
}


struct Line : QWidget
{
    Line()
    {
        setFixedHeight( 1 );
    }
    
    virtual void paintEvent( QPaintEvent* )
    {
        QPainter( this ).fillRect( 0, 0, width(), 1, QColor( 0x3a, 0x3b, 0x3c ) );
    }
};


struct ListView : QListWidget
{
    ListView()
    {
        QPalette p = palette();
        p.setColor( QPalette::AlternateBase, QColor( 0x1d, 0x1d, 0x1e ) );
        p.setColor( QPalette::Text, Qt::white );
        setPalette( p );
        setAlternatingRowColors( true );
        setAttribute( Qt::WA_MacShowFocusRect, false );
    }
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
    v->addWidget( ui.artist_text = new QLabel );
    v->addSpacing( 10 );
    v->addWidget( ui.bio = new mxcl::TextBrowser);
    v->addSpacing( 10 );
    v->addWidget( new Line );
    v->addSpacing( 10 );
    v->addWidget( new QLabel( HEADING "Tags" ) );
    v->addWidget( ui.tags = new ListView );
    v->addSpacing( 10 );
    v->addWidget( new Line );
    v->addSpacing( 10 );
    v->addWidget( new QLabel( HEADING "Similar Artists" ) );
    v->addWidget( ui.similarArtists = new ListView );
    v->addWidget( new Line );
    v->addSpacing( 10 );
    v->setMargin( 0 );
    v->setSpacing( 0 );

    ui.scrollbar = new FadingScrollBar( this );
    ui.scrollbar->setVisible( false );
    connect( ui.scrollbar, SIGNAL(valueChanged( int )), SLOT(setPapyrusPosition( int )) );
        
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

    ui.bio->document()->setDefaultFont( font );
    ui.bio->setOpenExternalLinks( true );
    ui.bio->document()->setDefaultStyleSheet( css );
    ui.bio->setAutoFillBackground( false );
    
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

        static QPointer<WsReply> r;
        delete r; //only one at a time please
        r = t.artist().getInfo();

        connect( r, SIGNAL(finished( WsReply* )), SLOT(onArtistGotInfo( WsReply* )) );
        
        ui.spinner->show();
        
        connect( t.artist().getTopTags(), SIGNAL(finished( WsReply* )), SLOT(onArtistGotTopTags( WsReply* )) );
    }

    ui.cover->setTrack( t );

    m_track = t;

    resizeEvent( 0 );
}


void
TrackDashboard::beginLoadingAnimation()
{
    ui.spinner->show();
}


void
TrackDashboard::clear()
{
    ui.cover->clear();
    ui.bio->clear();
    ui.scrollbar->setRange( 0, 0 );
    ui.artist_text->clear();
	ui.spinner->hide();
    ui.info->hide();
                 
    qDeleteAll( findChildren<WsReply*>() );
    
    m_track = Track();
}


void
TrackDashboard::onArtistGotInfo( WsReply* reply )
{
    qDebug() << reply;
    
    ui.spinner->hide();
    
	try
    {
		CoreDomElement e = reply->lfm()["artist"];
		QString name = e["name"].text();
		QString url = e["url"].text();
		QString	plays = e["stats"]["playcount"].text();
		QString listeners = e["stats"]["listeners"].text();
		QString content = e["bio"]["content"].text();
		QString editmessage = tr("Edit it too!");

		QString html;        
        if (content.isEmpty())
        {
            // this should be all one tr, but meh.
            html = tr("We don't have a description for this artist yet.") + "<a href='" + 
                   url + "/+wiki/edit'>" + tr("Why not write one?") + "</a>";
        }
        else
            QTextStream( &html ) <<
                    "<p id=content>" << content.replace(QRegExp("\r+"), "<p>") << "</p>"
                    "<p id=editme style='margin-top:0'>" << tr("This information was created by users like you! ") <<
                    "<a href=\"" << url << "/+wiki/edit" << "\">" << editmessage << "</a>";
        ui.bio->setHtml( html );
        resizeEvent( 0 );
        
        ui.artist_text->setText( HEADING + name + "</b></div>" + 
                                 "<div style='color:#a3a5a8;font-size:small'>" +
                                 tr( "%L1 listeners").arg( listeners.toUInt() ) + "<br>" + 
                                 tr( "%L1 plays").arg( plays.toUInt() ) );
        
        QNetworkRequest request( e["image size=large"].text() );
        QNetworkReply* reply = nam->get( request );
        connect( reply, SIGNAL(finished()), SLOT(onArtistImageDownloaded()) );

        foreach (CoreDomElement artist, e["similar"].children( "artist" ))
            ui.similarArtists->addItem( artist["name"].text() );
        
        ui.info->show();
        
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
    WeightedStringList tags = Tag::list( reply );
    for (int x = 0, n = qMin( tags.size(), 8 ); x < n; ++x)
        ui.tags->addItem( tags[x] );
}


void
TrackDashboard::onArtistImageDownloaded()
{
    return;
    
    QByteArray data = ((QNetworkReply*)sender())->readAll();
    QPixmap p;
    p.loadFromData( data );
    
    p = p.scaledToHeight( ui.artist->height(), Qt::SmoothTransformation );
    
    QPixmap p2( p.width(), p.height() + 13 );
    p2.fill( Qt::transparent );
    
    QPainter painter( &p2 );
    painter.drawPixmap( 0, 13, p );
    painter.setPen( Qt::black );
    painter.drawRect( 0, 13, p2.width() - 1, p.height() - 1 );

    ui.artist->setFixedSize( p2.size() );
    ui.artist->setPixmap( p2 );

    sender()->deleteLater();
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
        ui.cover->setShowArtist( false );
        ui.cover->move( 12, 12 );

        int h = height() - 12;
        ui.cover->resize( ui.cover->widthForHeight( h ), h );
        
        ui.info->move( ui.cover->geometry().right() + 12, 0 );
        
        w = width() - ui.info->geometry().x() - ui.scrollbar->sizeHint().width() - 12;
    }
    else
    {
        ui.scrollbar->fadeOutLater();
        ui.cover->setParent( ui.papyrus );
        ui.cover->move( 0, 0 );
        ui.cover->show();
        ui.cover->setShowArtist( true );
        ui.cover->raise();
        
        w = qMax( ui.cover->widthForHeight( height() - 12 ), 180 );
        int const x = (width() - w) / 2;

        ui.cover->resize( ui.papyrus->width(), height() - 12 );
        ui.info->move( x, ui.cover->height() );
    }

    ui.info->setFixedWidth( w );
    ui.bio->document()->setTextWidth( w );
    ui.bio->setFixedSize( ui.bio->sizeHint() );
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
    if (!m_track.isNull())
        return;
    
    QPainter p( this );
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

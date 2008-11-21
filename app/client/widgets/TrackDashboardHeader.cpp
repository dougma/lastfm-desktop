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

#include "TrackDashboardHeader.h"
#include <QHBoxLayout>
#include "widgets/PlaylistDialog.h"
#include "widgets/ScrobbleButton.h"
#include "widgets/RadioProgressBar.h"
#include "widgets/ImageButton.h"
#include "widgets/UnicornWidget.h"
#include "widgets/ShareDialog.h"
#include "widgets/TagDialog.h"
#include "lib/lastfm/ws/WsAccessManager.h"
#include "the/radio.h"
#include <QPainter>
#include <QMenu>


struct ThreeColumnLayout : QHBoxLayout
{
    ThreeColumnLayout( QWidget* parent ) : QHBoxLayout( parent ), middle( 0 )
    {}
    
    void setMiddle( QWidget* w )
    {
        middle = w;
        middle->setParent( parentWidget() );
        addStretch();
    }
    
    QWidget* middle;
    
private:
    virtual void setGeometry( const QRect& rr )
    {        
        QHBoxLayout::setGeometry( rr );
        
        int const w = middle->sizeHint().width();
        
        QRect r = parentWidget()->rect();
        int const x = (r.width() - w) / 2;
        middle->setGeometry( x, 0, w, r.height() );
    }
};



TrackDashboardHeader::TrackDashboardHeader()
{
    new ThreeColumnLayout( this );
    layout()->addWidget( ui.scrobbleButton = new ScrobbleButton );
    {
        QWidget* w = new QWidget;
        QBoxLayout* l = new QVBoxLayout( w );
        l->setSpacing( 0 );
        l->setContentsMargins( 0, 0, 0, 0 );
        l->addWidget( ui.track = new QLabel );
	#ifndef WIN32
        QFont f = ui.track->font();
        f.setPointSize( 11 );
        f.setBold( true );
        ui.track->setFont( f );
	#endif
        ui.track->setAlignment( Qt::AlignCenter );
        
        QPalette p = ui.track->palette();
        p.setBrush( QPalette::Text, QColor( 0xcccccc ));
        ui.track->setForegroundRole( QPalette::Text );
        ui.track->setPalette( p );
        
        UnicornWidget::paintItBlack( ui.track );
        
        l->addWidget( ui.radioProgress = new RadioProgressBar, 0, Qt::AlignCenter );
        ((ThreeColumnLayout*)layout())->setMiddle( w );
    }
    layout()->addWidget( ui.love = new ImageButton( ":/MainWindow/button/love/up.png" ));
    layout()->addWidget( ui.ban = new ImageButton( ":/MainWindow/button/ban/up.png" ));
    layout()->addWidget( ui.cog = new ImageButton( ":/MainWindow/button/cog/up.png" ));
    
    ui.ban->moveIcon( 0, 1, QIcon::Active );
    ui.love->moveIcon( 0, 1, QIcon::Active );
    ui.love->setPixmap( QPixmap(":/MainWindow/button/love/checked.png"), QIcon::On );
    ui.love->setCheckable( true );
    
    setContentsMargins( 6, 0, 10, 0 );
    layout()->setMargin( 0 );
    layout()->setSpacing( 10 );
    
    connect( ui.cog, SIGNAL(pressed()), SLOT(onCogMenuClicked()) );
    connect( this, SIGNAL( customContextMenuRequested( const QPoint& )), SLOT( onContextMenuRequested( const QPoint& )));

    setAutoFillBackground( true );
    setFixedHeight( 38 );
    
    connect( qApp, SIGNAL( trackSpooled( const Track&, StopWatch* )), SLOT( onTrackSpooled( const Track&, StopWatch* )));

    //Set ui to correct initial state
    onTrackSpooled( Track(), 0 );
}


void
TrackDashboardHeader::setCogMenu( QMenu* menu )
{
    m_cogMenu = menu;
    m_cogMenu->addSeparator();
    m_cogMenu->addAction( tr( "Praise the Client Team" ), this, SLOT( onPraiseClientTeam()) );
    m_cogMenu->addAction( tr( "Gently, Curse the Client Team" ), this, SLOT( onCurseClientTeam()) );
}


void 
TrackDashboardHeader::paintEvent( QPaintEvent* e )
{
    QWidget::paintEvent( e );
    
    QPainter p( this );
    p.setPen( Qt::black );
    p.drawLine( 0, height()-1, width(), height()-1 );
}


void 
TrackDashboardHeader::resizeEvent( QResizeEvent* e )
{
    QLinearGradient lg( 0, 0, 0, e->size().height() );
    lg.setColorAt( 0, 0x3a3a3a );
    lg.setColorAt( 0.45, 0x191919 );
    lg.setColorAt( 0.48, 0x0b0b0b );
    lg.setColorAt( 1, 0x0b0b0b );
    
    QPalette p = palette();
    p.setBrush( QPalette::Window, lg );
    setPalette( p );
}


void 
TrackDashboardHeader::onTrackSpooled( const Track& t, class StopWatch* )
{
    if( t.isNull() )
    {
        ui.track->clear();
        ui.cog->setEnabled( false );
    }
    else
    {
        ui.track->setText( t.toString() );
        ui.cog->setEnabled( true );
    }
}


void
TrackDashboardHeader::onCogMenuClicked()
{
    emit customContextMenuRequested( ui.cog->mapToGlobal( QPoint( ui.cog->geometry().width() - m_cogMenu->sizeHint().width(), ui.cog->height()) ) );
}


void 
TrackDashboardHeader::onContextMenuRequested( const QPoint& pos )
{
    m_cogMenu->move( pos );
    m_cogMenu->show();
}


#include <QtNetwork>
#include "lib/lastfm/types/User.h"
void
TrackDashboardHeader::onPraiseClientTeam()
{
    QStringList praises;
    praises << "thinks you're all jolly good chaps"
            << "wants to have your babies"
            << "is amazed at your skills"
            << "is gay and/or heterosexual for you (depending on gender)";
    
    uint random = QDateTime::currentDateTime().toTime_t();
    QString praise = praises.value( random % praises.count() );
    
	QUrl url = "http://oops.last.fm/talk/" + AuthenticatedUser() + " " + praise;
	(new WsAccessManager)->get( QNetworkRequest( url ) );
}


void
TrackDashboardHeader::onCurseClientTeam()
{
    QUrl url;
    if (Ws::Username == "mxcl")
        url = "http://oops.last.fm/talk/mxcl is teh ghae";
    else
        url = "http://oops.last.fm/talk/" + AuthenticatedUser() + " thinks y'all suck";

	(new QNetworkAccessManager)->get( QNetworkRequest( url ) );
}

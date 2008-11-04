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

#include "CogButtonPopup.h"
#include "widgets/UnicornWidget.h"
#include "lib/lastfm/ws/WsAccessManager.h"
#include <QCoreApplication>
#include <QPainter>
#include <QTimeLine>
#include <QVBoxLayout>
#include <QPushButton>


CogButtonPopup::CogButtonPopup( int width, QWidget* parent ) : QWidget( parent )
{
    QPushButton* cancel, *playlist, *praise, *curse;
    
    QVBoxLayout* v = new QVBoxLayout( this );
    v->addWidget( playlist = new QPushButton( tr("Add to Playlist") ) );
    v->addSpacing( 8 );
    v->addWidget( praise = new QPushButton( tr("Praise the Client Team") ) );
    v->addWidget( curse = new QPushButton( tr("Gently, Curse the Client Team") ) );
    v->addSpacing( 8 );
    v->addWidget( cancel = new QPushButton( tr("Cancel") ) );

	connect( playlist, SIGNAL(clicked()), SIGNAL(addToPlaylistClicked()) );
	connect( praise, SIGNAL(clicked()), SLOT(praise()) );
	connect( curse, SIGNAL(clicked()), SLOT(curse()) );
    
    foreach (QPushButton* b, findChildren<QPushButton*>())
        connect( b, SIGNAL(clicked()), SLOT(bye()) );
    
    // because the actionbar doesn't set its width correctly
    width = 270;
    
    adjustSize();
    resize( width, height() );
    QWidget::move( parent->rect().center().x() - width/2, 0 );
    move( 0 );
    
    m_timeline = new QTimeLine( 300, this );
    m_timeline->setCurveShape( QTimeLine::EaseInOutCurve );
    m_timeline->setFrameRange( 0, height() + 9 );
    connect( m_timeline, SIGNAL(frameChanged( int )), SLOT(move( int )) );
    m_timeline->start();
}


void
CogButtonPopup::bye()
{
    m_timeline->setFrameRange( m_timeline->currentFrame(), 0 );
    m_timeline->start();
    connect( m_timeline, SIGNAL(finished()), SLOT(deleteLater()) );
}


void 
CogButtonPopup::paintEvent( QPaintEvent* )
{
    QColor c( Qt::black );
    c.setAlphaF( 0.87 );
    
    QPainter p( this );
    p.setPen( Qt::NoPen );
    p.setBrush( c );
    p.setRenderHint( QPainter::Antialiasing );
    p.drawRoundedRect( rect(), 6, 6 );
}


#include <QtNetwork>
#include "lib/lastfm/types/User.h"
void
CogButtonPopup::praise()
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
CogButtonPopup::curse()
{
	QUrl url = "http://oops.last.fm/talk/" + AuthenticatedUser() + " thinks y'all suck";
	(new QNetworkAccessManager)->get( QNetworkRequest( url ) );
}

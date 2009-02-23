/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Christian Muehlhaeuser, Last.fm Ltd <chris@last.fm>                *
 *      Max Howell, Last.fm Ltd <max@last.fm>                              *
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

#include "RestStateMessage.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPalette>
#include <QPushButton>
#include <QTimeLine>
#include <QTimer>
#include <QToolTip>

class RestStateWidget;


RestStateMessage::RestStateMessage( RestStateWidget* parent )
        : QWidget( (QWidget*)parent )
{
    setAutoFillBackground( true );
    setBackgroundRole( QPalette::Window );

    QHBoxLayout* h = new QHBoxLayout( this );
    h->addWidget( ui.icon = new QLabel );
    h->addWidget( ui.text = new QLabel );

//TODO    ImageButton* faq = new ImageButton;
//TODO    faq->setPixmap( QPixmap( ":/button/MoreHelp.png" ) );
//TODO    faq->setPixmapDown( QPixmap( ":/button/MoreHelpActive.png" ) );
//TODO    faq->setPixmapHover( QPixmap( ":/button/MoreHelpHover.png" ) );
//TODO    faq->setCursor( Qt::PointingHandCursor );
//TODO    h->addWidget( faq );
//TODO    connect( faq, SIGNAL(clicked()), SIGNAL(moreHelpClicked()) );

    h->addWidget( ui.accept = new QPushButton );
    h->setSpacing( 6 );

//TODO    ImageButton* close = new ImageButton;
//TODO    QPixmap const q( ":/button/Close.png" );
//TODO    close->setPixmap( q );
//TODO    close->setPixmapDown( QPixmap( ":/button/CloseActive.png" ) );
//TODO    h->addWidget( close );
//TODO    connect( close, SIGNAL(clicked()), SLOT(deleteLater()) );

    QMessageBox box;
    box.setIcon( QMessageBox::Question );
    ui.icon->setPixmap( box.iconPixmap() );
    // add 6px spacing to icon width for aesthetics
    ui.icon->setFixedSize( ui.icon->sizeHint() + QSize( 6, 0 ) );

  #ifdef Q_WS_MAC
    QPalette p = palette();
    p.setColor( QPalette::Window, QColor( 0xe9, 0xe9, 0xe9 ) );
    p.setColor( QPalette::WindowText, QColor( 0x59, 0x59, 0x59 ) ); 
    setPalette( p );
    delete ui.icon; //there is no graphic on mac! *shrug*
    QFont f = ui.text->font();
    f.setPointSize( 11 );
    ui.text->setFont( f );
    ui.text->setPalette( p );
  #else
    setPalette( QToolTip::palette() );
  #endif

    ui.text->setWordWrap( true );
    ui.accept->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Fixed );

    m_timeline = new QTimeLine( 500 /*duration in ms*/, this );
    m_timeline->setUpdateInterval( 10 );
    connect( m_timeline, SIGNAL(frameChanged( int )), SLOT(onSlideStep( int )) );
    connect( m_timeline, SIGNAL(finished()), SLOT(onSlideFinished()) );

    connect( ui.accept, SIGNAL(clicked()), SIGNAL(accepted()) );

    //explicitly hide or parent() shows us
    move( 3000, 3000 );
}


void
RestStateMessage::setMessage( const QString& text )
{
    ui.text->setText( text );
}


void
RestStateMessage::setAcceptText( const QString& text )
{
    ui.accept->setText( text );
}


void
RestStateMessage::onSlideStep( int y )
{
    move( 0, parentWidget()->height() - y );
}


void
RestStateMessage::onSlideFinished()
{
    parentWidget()->layout()->addWidget( this );
    m_timeline->deleteLater();
    m_timeline = 0;
}


void
RestStateMessage::paintEvent( QPaintEvent* e )
{
    QWidget::paintEvent( e );

    QPainter p( this );
    p.setPen( Qt::gray );
    p.drawLine( QPoint(), QPoint( width(), 0 ) );
}


void
RestStateMessage::showEvent( QShowEvent* )
{
    if (m_timeline)
    {
        QTimer::singleShot( 2000, m_timeline, SLOT(start()) );

        int const w = parentWidget()->width();
        int const h = heightForWidth( w );
        resize( w, h );
        m_timeline->setFrameRange( 0, h );
    }
}

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

#include "ScrobbleViewWidget.h"
#include "MetaInfoView.h"
#include "ScrobbleInfoWidget.h"
#include "widgets/ImageButton.h"
#include "ui_MainWindow.h"

ScrobbleViewWidget::ScrobbleViewWidget()
{
	ScrobbleInfoWidget* w = ui.siw = new ScrobbleInfoWidget;
    
	QHBoxLayout* h = new QHBoxLayout( w->ui.actionbar );
    h->addWidget( ui.love = new ImageButton( ":/MainWindow/love.png" ) );
	h->addWidget( ui.ban = new ImageButton( ":/MainWindow/ban.png" ) );
	h->addWidget( ui.cog = new ImageButton( ":/MainWindow/cog_button.png") );
    h->addWidget( ui.tag = new ImageButton( ":/MainWindow/tag.png" ) );
    h->addWidget( ui.share = new ImageButton( ":/MainWindow/share.png" ) );   
	h->setSpacing( 24 );
    h->setSizeConstraint( QLayout::SetFixedSize );
    
    connect( ui.cog, SIGNAL(clicked()), SLOT(popupMultiButtonWidget()) );
	
    ui.love->setPixmap( ":/MainWindow/unlove.png", QIcon::On );
	ui.love->setCheckable( true );
    
#if 0
	QSplitter* s = new PaintedSplitter( Qt::Vertical );
	s->addWidget( w );
	s->addWidget( new MetaInfoView );
	s->setStretchFactor( 0, 0 );
	s->setStretchFactor( 1, 1 );
	s->setHandleWidth( 14 );
	
	QHBoxLayout* l = new QHBoxLayout( this );
    l->setMargin( 0 );
	l->addWidget( s );
#endif

	QHBoxLayout* l = new QHBoxLayout( this );
    l->setMargin( 0 );
	l->addWidget( w );
}


void
ScrobbleViewWidget::popupMultiButtonWidget()
{
    qDebug() << "hi";
    (new MultiButtonPopup( ui.siw->sizeHint().width(), this ))->show();
}


#include <QTimeLine>
#include <QVBoxLayout>
#include <QPushButton>
#include "widgets/UnicornWidget.h"

MultiButtonPopup::MultiButtonPopup( int width, QWidget* parent ) : QWidget( parent )
{
    QPushButton* cancel;
    
    QVBoxLayout* v = new QVBoxLayout( this );
    v->addWidget( new QPushButton( tr("Add to Playlist") ) );
    v->addWidget( new QPushButton( tr("Praise the Client Team") ) );
    v->addSpacing( 8 );
    v->addWidget( cancel = new QPushButton( tr("Cancel") ) );
    
    connect( cancel, SIGNAL(clicked()), SLOT(bye()) );
    
    
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
MultiButtonPopup::bye()
{
    m_timeline->setFrameRange( m_timeline->currentFrame(), 0 );
    m_timeline->start();
    connect( m_timeline, SIGNAL(finished()), SLOT(deleteLater()) );
}


void 
MultiButtonPopup::paintEvent( QPaintEvent* )
{
    QColor c( Qt::black );
    c.setAlphaF( 0.87 );

    QPainter p( this );
    p.setPen( Qt::NoPen );
    p.setBrush( c );
    p.setRenderHint( QPainter::Antialiasing );
    p.drawRoundedRect( rect(), 6, 6 );
}

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
#include "MediaPlayerIndicator.h"
#include "NowPlayingView.h"
#include "widgets/ScrobbleProgressBar.h"
#include <QHBoxLayout>

ScrobbleViewWidget::ScrobbleViewWidget()
{
	ui.actionbar = new QWidget;
	
	QVBoxLayout* v = new QVBoxLayout( this );
	v->addWidget( ui.playerIndicator = new MediaPlayerIndicator );
	v->addSpacing( 8 );
    v->addWidget( ui.cover = new NowPlayingView );
	v->addSpacing( 4 );
    v->addWidget( ui.actionbar );
	v->addSpacing( 12 );
    v->addWidget( ui.progress = new ScrobbleProgressBar );
	v->setSpacing( 0 );

    v->setContentsMargins( 10, 8, 10, 14 );
    v->setAlignment( ui.actionbar, Qt::AlignCenter );
	
	uint const W = ui.actionbar->sizeHint().width() + 20;
    setMinimumWidth( W );
	
	QPalette p( Qt::white, Qt::black );
    setPalette( p );
    setAutoFillBackground( true );
	
	setMinimumHeight( sizeHint().height() );
}


void
ScrobbleViewWidget::resizeEvent( QResizeEvent* )
{
//	QLinearGradient g( 0, ui.cover->height()*5/7 + 10 /*margin*/, 0, height() );
	QLinearGradient g( 0, height() - 300, 0, height() );
	g.setColorAt( 0, Qt::black );
	g.setColorAt( 1, QColor( 0x30, 0x2e, 0x2e ) );

	QPalette p = palette();
	p.setBrush( QPalette::Window, g );
	
	//inefficient as sets recursively on child widgets? 
	//may be better to just paintEvent it
	setPalette( p );
}

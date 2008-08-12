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
#include "widgets/MediaPlayerIndicator.h"
#include "widgets/NowPlayingView.h"
#include "widgets/ScrobbleProgressBar.h"
#include <QHBoxLayout>

ScrobbleViewWidget::ScrobbleViewWidget()
{
	ui.actionbar = new QWidget;
	
	QVBoxLayout* v = new QVBoxLayout( this );
	v->addWidget( ui.playerIndicator = new MediaPlayerIndicator );
    v->addWidget( ui.cover = new NowPlayingView );
    v->addWidget( ui.progress = new ScrobbleProgressBar );
    v->addWidget( ui.actionbar );
    v->setMargin( 10 );
    v->setAlignment( ui.actionbar, Qt::AlignCenter );
	
	uint const W = ui.actionbar->sizeHint().width() + 20;
    setMinimumWidth( W );
	
	QPalette p( Qt::white, Qt::black );
    setPalette( p );
    setAutoFillBackground( true );
    
    adjustSize(); //because Qt sucks? It uses the UI size initially I think
	
}

void
ScrobbleViewWidget::resizeEvent( QResizeEvent* )
{
	QLinearGradient g( 0, ui.cover->height()*5/7 + 10 /*margin*/, 0, height() );
	g.setColorAt( 0, Qt::black );
	g.setColorAt( 1, QColor( 0x20, 0x20, 0x20 ) );
	
	QPalette p = palette();
	p.setBrush( QPalette::Window, g );
	
	//inefficient as sets recursively on child widgets? 
	//may be better to just paintEvent it
	setPalette( p );        
	
}
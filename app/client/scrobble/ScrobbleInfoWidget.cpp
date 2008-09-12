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

#include "ScrobbleInfoWidget.h"
#include "MediaPlayerIndicator.h"
#include "TrackInfoWidget.h"
#include "the/definitions.h"
#include "lib/unicorn/widgets/SpinnerLabel.h"
#include "widgets/ScrobbleProgressBar.h"
#include <QCoreApplication>
#include <QVBoxLayout>
#include <QPainter>
#include <QPaintEvent>


ScrobbleInfoWidget::ScrobbleInfoWidget()
{
	ui.actionbar = new QWidget;
	
	QVBoxLayout* v = new QVBoxLayout( this );
	v->addWidget( ui.playerIndicator = new MediaPlayerIndicator );
	v->addSpacing( 8 );
    v->addWidget( ui.cover = new TrackInfoWidget );
	v->addSpacing( 4 );
    v->addWidget( ui.progress = new ScrobbleProgressBar );
	v->addSpacing( 0 );
    v->addWidget( ui.actionbar );
	v->setSpacing( 0 );
	v->setSizeConstraint( QLayout::SetMinimumSize );

    v->setContentsMargins( 10, 8, 10, 5 );
    v->setAlignment( ui.actionbar, Qt::AlignCenter );
	
	uint const W = ui.actionbar->sizeHint().width() + 20;
    setMinimumWidth( W );
	
	QPalette p( Qt::white, Qt::black );
    setPalette( p );
    setAutoFillBackground( true );
	
	setMinimumHeight( sizeHint().height() );
	
	connect( qApp, SIGNAL(trackSpooled( Track )), SLOT(onTrackSpooled( Track )) );
	connect( qApp, SIGNAL(stateChanged( State )), SLOT(onStateChanged( State )) );
}


void
ScrobbleInfoWidget::resizeEvent( QResizeEvent* )
{
	QRadialGradient g( width() / 2, 326, float(width()) / 1.1f, width() / 2, 180 );
	qDebug() << height();
	g.setColorAt( 1, Qt::black );
	g.setColorAt( 0, QColor( 0x30, 0x2e, 0x2e ) );

	QPalette p = palette();
	p.setBrush( QPalette::Window, g );
	
	//inefficient as sets recursively on child widgets? 
	//may be better to just paintEvent it
	setPalette( p );
}


void
ScrobbleInfoWidget::paintEvent( QPaintEvent* e )
{
	QPainter p( this );
	p.setClipRect( e->rect() );
	QRect radialRect = rect();
	radialRect.setHeight( 326 );
	
	p.fillRect( radialRect, palette().brush( QPalette::Window));
	
	QLinearGradient lgradient;
	lgradient.setColorAt( 0, QColor( 0x30, 0x2e, 0x2e) );
	lgradient.setColorAt( 1, Qt::black );

	radialRect.setTop( rect().top() + 326 );
	radialRect.setHeight( rect().height() - 326 );

	p.fillRect( radialRect, lgradient );
}


void 
ScrobbleInfoWidget::onTrackSpooled( const Track& t )
{
    if (!t.isNull())
    {
        ui.cover->setTrack( t );
    }
    else
        ui.cover->clear();
}


void
ScrobbleInfoWidget::onStateChanged( State s )
{
	switch (s)
	{            
        case TuningIn:
			ui.cover->ui.spinner->show();
			break;
            
        default:
            break;
	}
}

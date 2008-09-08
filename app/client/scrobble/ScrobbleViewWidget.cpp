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

ScrobbleViewWidget::ScrobbleViewWidget( Ui::MainWindow& ui, QWidget* parent )
				   :QWidget( parent )
{

	ScrobbleInfoWidget* w = new ScrobbleInfoWidget;
	QHBoxLayout* h = new QHBoxLayout( w->ui.actionbar );
    h->addStretch();
    h->addWidget( new SimpleButton( ":/MainWindow/love.png", ui.love ) );
    h->addWidget( new SimpleButton( ":/MainWindow/ban.png", ui.ban ) );
    h->addWidget( new SimpleButton( ":/MainWindow/tag.png", ui.tag ) );
    h->addWidget( new SimpleButton( ":/MainWindow/share.png", ui.share ) );
 
	h->setSpacing( 0 );
    h->setMargin( 0 );

    h->setSizeConstraint( QLayout::SetFixedSize );
	
	QSplitter* s = new PaintedSplitter( Qt::Vertical );
	s->addWidget( w );
	s->addWidget( new MetaInfoView );
	s->setSizes( QList<int>() << 80 << 80 );
	s->setStretchFactor( 0, 0 );
	s->setStretchFactor( 1, 1 );
	s->setHandleWidth( 14 );
	
	QHBoxLayout* l = new QHBoxLayout( this );
	
	l->setContentsMargins( 0, 0, 0, 0);
	l->addWidget( s );
	
}
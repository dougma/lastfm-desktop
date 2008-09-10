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

ScrobbleViewWidget::ScrobbleViewWidget( Ui::MainWindow& mainUi, QWidget* parent )
				   :QWidget( parent )
{

	ScrobbleInfoWidget* w = new ScrobbleInfoWidget;
	QHBoxLayout* h = new QHBoxLayout( w->ui.actionbar );

    h->addWidget( ui.love = new ImageButton( ":/MainWindow/love.png", mainUi.love) );
	ui.love->setCheckedIcon( QIcon( ":/MainWindow/unlove.png" ));
	ui.love->setCheckable( true );
	h->addWidget( ui.ban = new ImageButton( ":/MainWindow/ban.png", mainUi.ban ));
	h->addWidget( ui.cog = new ImageButton( ":/MainWindow/cog_button.png"));
    h->addWidget( ui.tag = new ImageButton( ":/MainWindow/tag.png", mainUi.tag ));
    h->addWidget( ui.share = new ImageButton( ":/MainWindow/share.png", mainUi.share));
 
	h->setSpacing( 40 );

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
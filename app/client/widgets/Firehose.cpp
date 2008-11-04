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

#include "Firehose.h"
#include "FirehoseDelegate.h"
#include "FirehoseModel.h"
#include "FirehoseView.h"
#include "widgets/UnicornWidget.h"
#include "widgets/UnicornTabWidget.h"
#include <QVBoxLayout>


Firehose::Firehose()
{
    Unicorn::TabBar* tabs;
    class FirehoseView* view;
    
    QVBoxLayout *v = new QVBoxLayout( this );
    //v->addWidget( tabs = new Unicorn::TabBar );
    v->addWidget( view = new FirehoseView );
    v->setMargin( 0 );
    v->setSpacing( 0 );
    
    FirehoseModel* model;
    view->setModel( model = new FirehoseModel );
    view->setDelegate( new FirehoseDelegate );

#if 0
    CoreSignalMapper* mapper = new CoreSignalMapper( this );
    mapper->setMapping( 0, "user/1000002?rt=xml&special=staffmembers" );
    mapper->setMapping( 1, "user/2113030?rt=xml" );
    connect( tabs, SIGNAL(currentChanged( int )), mapper, SLOT(map( int )) );
    connect( mapper, SIGNAL(mapped( QString )), model, SLOT(setNozzle( QString )) );
    
    tabs->addTab( tr("Last.fm Staff") );
    tabs->addTab( tr("All your Friends") );
    
    //tabs->succombToTheDarkSide();
#else
    model->setNozzle( "user/1000002?rt=xml&special=staffmembers" );
    setMinimumWidth( 250 );
    Q_UNUSED( tabs );
#endif

    UnicornWidget::paintItBlack( this );
    QPalette p = palette();
    p.setBrush( QPalette::Base, p.window() );
    p.setBrush( QPalette::Text, p.windowText() );
    p.setBrush( QPalette::AlternateBase, QColor( 39, 38, 38 ) );
    setPalette( p );
    view->setPalette( palette() );
    view->setAutoFillBackground( true );
}



QSize
Firehose::sizeHint() const
{
    return QSize( 358, 600 );
}

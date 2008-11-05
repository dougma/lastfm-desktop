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

#include "Amp.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "Amp.h"
#include "widgets/RadioControls.h"
#include "widgets/UnicornWidget.h"
#include "the/radio.h"
#include "PlayableListItem.h"
#include "PlayerBucketList.h"

#include <QPaintEvent>
#include <QPainter>

Amp::Amp( QWidget* p )
    :QWidget( p )
{
    setAutoFillBackground( true );
    new QHBoxLayout( this );
    
    layout()->setContentsMargins( 0, 0, 0, 0 );
    
    struct BorderedContainer : public QWidget
    {
        BorderedContainer( QWidget* parent = 0 ) : QWidget( parent ){ setLayout( new QVBoxLayout ); layout()->setContentsMargins( 15, 15, 15, 15 ); setAutoFillBackground( true ); }
        
        void paintEvent( QPaintEvent* e )
        {
            QPainter p( this );
            p.setClipRect( e->rect());
            p.setRenderHint( QPainter::Antialiasing, true );
            QPen pen( QBrush(0x4e4e4e), 3, Qt::DashLine, Qt::RoundCap );
            pen.setDashPattern( QVector<qreal>() << 5 << 2 );
            pen.setWidth( 1 );
            p.setPen( pen );
            p.drawRoundedRect( rect().adjusted( 10, 10, -10, -10), 10, 10 );
        }
        
        void resizeEvent( QResizeEvent* e )
        {
            QLinearGradient g( 0, 0, 0, height() );
            g.setColorAt( 0, 0x070707 );
            g.setColorAt( 1, 0x1b1a1a );
            
            QPalette p = palette();
            p.setBrush( QPalette::Window, g );
            setPalette( p );
            
        }
        
        void moveEvent( QMoveEvent* e )
        {
            Q_UNUSED( e )
        }
        
    };
    
    layout()->addWidget( ui.controls = new RadioControls );
    ui.controls->setAutoFillBackground( false );
    ui.controls->layout()->setSizeConstraint( QLayout::SetFixedSize );
    
    QWidget* bucketLayoutWidget = new QWidget( this );
    BorderedContainer* bc = new BorderedContainer( this );
    bc->layout()->addWidget( ui.bucket = new PlayerBucketList( bc ));
    layout()->addWidget( bc );
    
    
    bc->setFixedHeight( 100 );
  
    
    layout()->addWidget( bucketLayoutWidget );
    
    connect( ui.bucket, SIGNAL( itemRemoved( QString, Seed::Type)), SIGNAL( itemRemoved( QString, Seed::Type)));
    
    
    connect( ui.controls, SIGNAL( skip()), &The::radio(), SLOT( skip()) );
    connect( ui.controls, SIGNAL( stop()), &The::radio(), SLOT( stop()));
    connect( ui.controls, SIGNAL( play()), ui.bucket, SLOT( play()));
    
    UnicornWidget::paintItBlack( this );
    UnicornWidget::paintItBlack( bucketLayoutWidget);
}


void 
Amp::addAndLoadItem( const QString& itemText, const Seed::Type type )
{
    PlayableListItem* item = new PlayableListItem;
    item->setText( itemText );
    item->setPlayableType( type );
	item->setForeground( Qt::white );
	item->setBackground( QColor( 0x2e, 0x2e, 0x2e));
	item->setFlags( item->flags() | Qt::ItemIsDragEnabled );
    item->fetchImage();
    ui.bucket->addItem( item );
    
}


void 
Amp::resizeEvent( QResizeEvent* event )
{
    QPalette p = palette();
    QLinearGradient lg( 0, 0, 0, event->size().height() );
    lg.setColorAt( 0, 0x5c5e60 );
    lg.setColorAt( 1, 0x2d2d2d );
    p.setBrush( QPalette::Window, lg );
    setPalette( p );
}
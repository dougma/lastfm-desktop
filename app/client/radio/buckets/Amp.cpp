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

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "Amp.h"
#include "widgets/RadioControls.h"
#include "widgets/UnicornWidget.h"
#include "widgets/UnicornVolumeSlider.h"
#include "the/radio.h"
#include "the/MainWindow.h"
#include "widgets/ImageButton.h"
#include "PlayableListItem.h"
#include "PlayerBucketList.h"

#include <QPaintEvent>
#include <QPainter>

Amp::Amp( QWidget* p )
    :QWidget( p )
{
    setupUi();
    connect( qApp, SIGNAL(trackSpooled( Track )), SLOT(onTrackSpooled( Track )) );
    
    onPlayerBucketChanged();
    
}


void 
Amp::setupUi()
{
    setAutoFillBackground( true );
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );
    new QHBoxLayout( this );
    
    layout()->setSpacing( 5 );
    layout()->setContentsMargins( 5, 6, 5, 8 );

    
    layout()->addWidget( ui.controls = new RadioControls );
    ui.controls->setAutoFillBackground( false );
    ui.controls->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred ) );
    
    struct BorderedContainer : public QWidget
    {
        BorderedContainer( QWidget* parent = 0 ) : QWidget( parent )
        { 
            setLayout( new QVBoxLayout ); 
            layout()->setContentsMargins( 3, 3, 3, 3 ); 
            setAutoFillBackground( false ); 
        }
        
        void paintEvent( QPaintEvent* e )
        {
            QPainter p( this );
            p.setClipRect( e->rect() );
            p.setRenderHint( QPainter::Antialiasing );

            p.setPen( Qt::transparent );
            p.setBrush( palette().brush( QPalette::Window ));
            p.drawRoundedRect( rect(), 3, 3 );
            p.setPen( QPen( palette().brush( QPalette::Shadow ), 0.5));
            p.drawRoundedRect( rect().adjusted( 0.5, 0.5, -0.5, -0.5), 3, 3);
        }
        
        void resizeEvent( QResizeEvent* e )
        {
            QLinearGradient window( 0, 0, 0, e->size().height() );
            window.setColorAt( 0, 0x0e0e0e );
            window.setColorAt( 1, 0x292828 );
            
            QLinearGradient shadow( 0, 0, 0, e->size().height() );
            shadow.setColorAt( 0, 0x0e0e0e );
            shadow.setColorAt( 1, 0x888888 );
                             
            QPalette p = palette();
            p.setBrush( QPalette::Window, window );
            p.setBrush( QPalette::Shadow, shadow );
            setPalette( p );
        }
        
    };
    
    QWidget* bucketLayoutWidget = new QWidget( this );
    BorderedContainer* bc = new BorderedContainer( this );
    bc->layout()->addWidget( ui.bucket = new PlayerBucketList( bc ));
    layout()->addWidget( bc );
        
    layout()->addWidget( bucketLayoutWidget );
    connect( ui.bucket, SIGNAL( itemAdded( QString, Seed::Type)), SLOT( onPlayerBucketChanged()));
    connect( ui.bucket, SIGNAL( itemRemoved( QString, Seed::Type)), SLOT( onPlayerBucketChanged()));
    
//    QWidget* scrobbleRatingControls = new QWidget( this );
//    {
//        QGridLayout* layout = new QGridLayout( scrobbleRatingControls );
//        layout->addWidget( scrobbleRatingUi.love = new ImageButton( ":/MainWindow/button/love/up.png" ), 0, 0, Qt::AlignRight | Qt::AlignBottom );
//        layout->addWidget( scrobbleRatingUi.ban = new ImageButton( ":/MainWindow/button/ban/up.png" ), 0, 1, Qt::AlignLeft | Qt::AlignBottom );
//        layout->addWidget( scrobbleRatingUi.tag = new ImageButton( ":/MainWindow/button/tag/up.png" ), 1, 0, Qt::AlignRight | Qt::AlignTop );
//        layout->addWidget( scrobbleRatingUi.share = new ImageButton( ":/MainWindow/button/share/up.png" ), 1, 1, Qt::AlignLeft | Qt::AlignTop );
//        
//        layout->setSizeConstraint( QLayout::SetFixedSize );
//        layout->setSpacing( 0 );
//        layout->setContentsMargins( 0, 0, 0, 0 );
//        
//        scrobbleRatingUi.ban->moveIcon( 0, 1, QIcon::Active );
//        scrobbleRatingUi.tag->moveIcon( 0, 1, QIcon::Active );
//        scrobbleRatingUi.love->moveIcon( 0, 1, QIcon::Active );
//        scrobbleRatingUi.love->setPixmap( ":/MainWindow/button/love/checked.png", QIcon::On );
//        scrobbleRatingUi.love->setCheckable( true );
//        scrobbleRatingUi.share->moveIcon( 0, 1, QIcon::Active );
//        
//    }
    
    
//    layout()->addWidget( scrobbleRatingControls );
    
//    QWidget* cogAndScrobble = new QWidget( this );
//    {
//        QVBoxLayout* layout = new QVBoxLayout( cogAndScrobble );
//        layout->setSizeConstraint( QLayout::SetFixedSize );
//        layout->setSpacing( 0 );
//        layout->setContentsMargins( 0, 0, 0, 0 );
//        layout->addWidget( scrobbleRatingUi.cog = new ImageButton( ":/MainWindow/button/cog/up.png" ));
//        scrobbleRatingUi.cog->setCheckable( true );
//        connect( scrobbleRatingUi.cog, SIGNAL(clicked()), SLOT(onCogMenuClicked()) );
//
//        m_cogMenu = new QMenu( this );
//        m_cogMenu->addAction( tr( "Add to playlist" ), this, SLOT( showPlaylistDialog()) );
//        m_cogMenu->addAction( tr( "Praise the Client Team" ), this, SLOT( onPraiseClientTeam()) );
//        m_cogMenu->addAction( tr( "Gently, Curse the Client Team" ), this, SLOT( onCurseClientTeam()) );
//        
//        
//        
//    }

    ui.volume = new UnicornVolumeSlider;
    ui.volume->setAudioOutput( The::radio().audioOutput());
    ui.volume->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred ) );
    layout()->addWidget( ui.volume );
    
    connect( ui.bucket, SIGNAL( itemRemoved( QString, Seed::Type)), SIGNAL( itemRemoved( QString, Seed::Type)));
    
    connect( ui.controls, SIGNAL( skip()), &The::radio(), SLOT( skip()) );
    connect( ui.controls, SIGNAL( stop()), &The::radio(), SLOT( stop()));
    connect( ui.controls, SIGNAL( play()), ui.bucket, SLOT( play()));
    
    ui.controls->setMinimumWidth( ui.volume->sizeHint().width());
    
    setMinimumWidth( 456 );
}


void 
Amp::onPlayerBucketChanged()
{
    if( ui.bucket->count() > 0 )
    {
        ui.controls->setFixedHeight( ui.controls->sizeHint().height());
        ui.volume->setFixedHeight( ui.volume->sizeHint().height());
    }
    else
    {
        ui.controls->setFixedHeight( 0 );
        ui.volume->setFixedHeight( 0 );
    }
    
    ui.volume->resize( ui.volume->size());
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
    QLinearGradient window( 0, 0, 0, event->size().height() );
    window.setColorAt( 0, 0x4c4a4a );
    window.setColorAt( 1, 0x282727 );
    p.setBrush( QPalette::Window, window );
    setPalette( p );
}

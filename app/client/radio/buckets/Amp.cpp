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
#include "widgets/ImageButton.h"
#include "Seed.h"
#include "PlayerBucketList.h"
#include <QPaintEvent>
#include <QPainter>
#include <QTimeLine>


struct BorderedContainer : public QWidget
{
    BorderedContainer( QWidget* parent = 0 ) : QWidget( parent ), m_showText( false )
    { 
        (new QHBoxLayout( this ))->setContentsMargins( 1, 1, 1, 1 );
        setAutoFillBackground( false ); 
        QPalette p = palette();
        p.setBrush( QPalette::Text, QBrush( 0x777777));
        setPalette( p );
    }
    
    void paintEvent( QPaintEvent* e )
    {
        QPainter p( this );
        p.setClipRect( e->rect() );
        p.setRenderHint( QPainter::Antialiasing );
        
        p.setPen( Qt::transparent );
        p.setBrush( palette().brush( QPalette::Window ));
        p.drawRoundedRect( rect(), 6, 6 );
        p.setPen( QPen( palette().brush( QPalette::Shadow ), 0));
        p.drawRoundedRect( rect(), 6, 6 );

        p.setPen( QPen( palette().brush( QPalette::Text).color()) );
        if( m_showText )
        {
		#ifndef WIN32
			QFont f = p.font();
		
            f.setBold( true );
            f.setPointSize( 12 );
			p.setFont( f );
		#endif
            
            p.drawText( rect(), 
                       Qt::AlignCenter | Qt::TextWordWrap, 
                       m_text );
        }
    }
    
    void showText( bool b ){ m_showText = b; update();}
    bool isTextShown() const { return m_showText; }
    
    void resizeEvent( QResizeEvent* e )
    {
        QLinearGradient window( 0, 0, 0, e->size().height() );
        window.setColorAt( 0, Qt::black );
        window.setColorAt( 1, 0x2b2929 );
        
        QLinearGradient shadow( 0, 0, 0, e->size().height() );
        shadow.setColorAt( 0, 0x0d0c0c );
        shadow.setColorAt( 0.5, 0x1c1b1b );
        shadow.setColorAt( 1, 0x5e5e5e );
        
        QPalette p = palette();
        p.setBrush( QPalette::Window, window );
        p.setBrush( QPalette::Shadow, shadow );
        setPalette( p );
    }
    
    void setText( const QString& s ){ m_text = s; update(); }
    
private: QString m_text; bool m_showText;
};


Amp::Amp()
{    
    setupUi();
    
    connect( qApp, SIGNAL(stateChanged( State )), SLOT(onStateChanged( State )) );
    connect( qApp, SIGNAL(playerChanged( QString )), SLOT(onPlayerChanged( QString )));

    m_timeline = new QTimeLine( 500, this );
    m_timeline->setUpdateInterval( 10 );
    connect( m_timeline, SIGNAL(frameChanged( int )), SLOT(onWidgetAnimationFrameChanged( int )));
    
    // we take the common factor of the two widths
    m_timeline->setFrameRange( 0, ui.controls->width() * ui.volume->width() );
    
    onPlayerBucketChanged();
}


void 
Amp::setupUi()
{
    new QHBoxLayout( this );
    
    layout()->setSpacing( 13 );
    layout()->setContentsMargins( 11, 9, 12, 11 );
    
    ui.borderWidget = new BorderedContainer( this );
    
    {
        //Radio and volume controls need to be embedded in a parent QWidget
        //in order to properly mask when animating out / into view.
        QWidget* controls = new QWidget(ui.borderWidget);
        new QHBoxLayout( controls );
        controls->layout()->setSpacing( 0 );
        controls->layout()->setContentsMargins( 0, 0, 0, 0 );
        controls->layout()->addWidget( ui.controls = new RadioControls );
        ui.borderWidget->layout()->addWidget( controls );
    }
    {
        QWidget* volume = new QWidget(ui.borderWidget);
        new QHBoxLayout( volume );
        volume->layout()->setSpacing( 0 );
        volume->layout()->setContentsMargins( 0, 0, 0, 0 );
        volume->layout()->addWidget( ui.volume = new UnicornVolumeSlider );
        ui.borderWidget->layout()->addWidget( volume );
    }
    
    ui.controls->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred ) );
    
    ((QBoxLayout*)ui.borderWidget->layout())->insertWidget( 1, ui.bucket = new PlayerBucketList( ui.borderWidget ));
    layout()->addWidget( ui.borderWidget );
        
    connect( ui.bucket, SIGNAL( itemAdded( QString, Seed::Type)), SLOT( onPlayerBucketChanged()));
    connect( ui.bucket, SIGNAL( itemRemoved( QString, Seed::Type)), SLOT( onPlayerBucketChanged()));

    ui.volume->setAudioOutput( The::radio().audioOutput());
    ui.volume->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred ) );

    connect( ui.bucket, SIGNAL( itemRemoved( QString, Seed::Type)), SIGNAL( itemRemoved( QString, Seed::Type)));
    
    connect( ui.controls, SIGNAL( skip()), &The::radio(), SLOT( skip()) );
    connect( ui.controls, SIGNAL( stop()), &The::radio(), SLOT( stop()));
    connect( ui.controls, SIGNAL( play()), ui.bucket, SLOT( play()));

    setFixedHeight( 86 );
    setAutoFillBackground( true );
}


void 
Amp::onPlayerBucketChanged()
{
    setRadioControlsVisible( ui.bucket->model()->rowCount() > 0 );
}


bool
Amp::isRadioControlsVisible() const
{
    if (m_timeline->state() == QTimeLine::Running)
        return m_timeline->direction() == QTimeLine::Backward;
    
    return ui.controls->x() == 0;
}


void
Amp::setRadioControlsVisible( bool b )
{
    if (b == isRadioControlsVisible())
        return;

    if (b)
    {
        m_timeline->setDirection( QTimeLine::Backward );
        m_timeline->start();
        ui.borderWidget->showText( false );
    }
    else 
    {
        m_timeline->setDirection( QTimeLine::Forward );
        m_timeline->start();
    }
}


void
Amp::addAndLoadItem( const QString& itemText, const Seed::Type type )
{
    Seed* item = new Seed;
    item->setName( itemText );
    item->setPlayableType( type );
    item->fetchImage();
    ui.bucket->addItem( item );
}


void 
Amp::resizeEvent( QResizeEvent* event )
{
    QPalette p = palette();
    QLinearGradient window( 0, 0, 0, event->size().height() );
    window.setColorAt( 0, 0x383737 );
    window.setColorAt( 1, 0x161616 );
    p.setBrush( QPalette::Window, window );
    setPalette( p );
    
    if( event->size().width() == event->oldSize().width() )
        return;
    
    if (!isRadioControlsVisible())
    {
        ui.controls->move( -ui.controls->rect().width(), 0);
        ui.volume->move( ui.volume->rect().width(), 0);
    }
        
}


void 
Amp::paintEvent( QPaintEvent* event )
{
    QPainter p( this );
    p.setClipRect( event->rect() );
    
    p.setPen( Qt::black );
    p.drawLine( rect().topLeft(), rect().topRight() );
    p.setPen( 0x4c4b4b );
    p.drawLine( rect().topLeft() + QPoint(0,1), rect().topRight() + QPoint(0,1) );
    p.setPen( Qt::black );
    p.drawLine( rect().bottomLeft(), rect().bottomRight() );
}


void 
Amp::onWidgetAnimationFrameChanged( int frame )
{
    QWidget* left = ui.controls;
    QWidget* right = ui.volume;
    int const xleft = 0 - frame / right->width();
    int const xright = (right->parentWidget()->width() - right->width()) + frame / left->width();

    left->move( xleft, left->y() );
    right->move( xright, right->y() );
}


void
Amp::onStateChanged( State s )
{
    switch ((int)s)
    {
        case TuningIn:
            m_playerName.clear();
            ui.borderWidget->showText( false );
            setRadioControlsVisible( true );
            break;

        case Paused:
            if (m_playerName.size())
                ui.borderWidget->setText( tr( "%1 is paused" ).arg( m_playerName ) );
            break;
            
        case Playing:
            if (m_playerName.size())
                ui.borderWidget->setText( tr("Scrobbling from %1").arg( m_playerName ) );
            break;
            
        case Stopped:
            setRadioControlsVisible( ui.bucket->model()->rowCount() > 0 );
            if (m_playerName.size())
                ui.borderWidget->setText( tr( "Connected to %1" ).arg( m_playerName ) );
            else
                ui.borderWidget->setText( "" );
    }
    

}


void 
Amp::onPlayerChanged( const QString& name )
{
    m_playerName = name;
    onStateChanged( Stopped );
    ui.borderWidget->showText( m_playerName.size() );
}

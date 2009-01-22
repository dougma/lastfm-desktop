/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
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

#include "ScrobbleButton.h"
#include "ScrobbleButton/PausableTimer.h"
#include "StopWatch.h"
#include "lib/lastfm/ws/WsConnectionMonitor.h"
#include <QApplication>
#include <QLabel>
#include <QPainter>
#include <QTimeLine>


class ScrobbleButtonToolTip : public QLabel
{
public:
	ScrobbleButtonToolTip( QWidget* parent ) : QLabel( parent, Qt::Window | Qt::FramelessWindowHint | Qt::ToolTip )
	{
		setWindowOpacity( 0.9 );
		setTextFormat( Qt::RichText );
		setAlignment( Qt::AlignCenter );
		
		QPalette p = palette();
		p.setColor( QPalette::WindowText, Qt::white );
		p.setColor( QPalette::Text, Qt::white );
		setPalette( p );
		
		setMargin( 10 );
	}
	
	virtual void paintEvent( QPaintEvent* e )
	{
		QPainter p( this );
		p.fillRect( rect(), QColor( 0x0e0e0e ) );
		QPen pen( Qt::white );
		pen.setWidth( 2 );
		p.setPen( pen );
		p.drawRect( rect().adjusted( 1, 1, -1, -1 ) );
		p.end();
        QLabel::paintEvent( e );
	}
};


ScrobbleButton::ScrobbleButton()
{
	m_tip = new ScrobbleButtonToolTip( this );
	
    m_connectionMonitor = new WsConnectionMonitor( this );
	connect( m_connectionMonitor, SIGNAL(connectivityChanged( bool )), SLOT(update()) );
    
    setCheckable( true );
    setChecked( true );
    setFixedSize( 51, 37 );
    
    connect( this, SIGNAL(toggled( bool )), SLOT(onChecked( bool )) );
    connect( qApp, SIGNAL(trackSpooled( Track, StopWatch* )), SLOT(onTrackSpooled( Track, StopWatch* )) );
}


void
ScrobbleButton::paintEvent( QPaintEvent* )
{
    QPixmap p;
    
    if (!isChecked()) {
        p = QPixmap( ":/ScrobbleButton/off.png" );
    }
    else if (m_scrobble.isNull()) { //no track playing
        p = QPixmap( ":/ScrobbleButton/on.png" );
    }
    else if (!m_scrobble.isValid()) {
        p = QPixmap( ":/ScrobbleButton/SADFACE.png" );
    }
    else
        p = m_pixmap;

    QPainter( this ).drawPixmap( rect(), p );
}


void
ScrobbleButton::onTrackSpooled( const Track& t, class StopWatch* watch )
{
    m_scrobble = t;
    delete m_animation;

    if (t.isNull())
    {
        m_tip->clear();
        update();
    }
    else if (m_scrobble.isValid())
    {
        m_scrobblePoint = watch->scrobblePoint();        
        
        m_animation = new ScrobbleButtonAnimation( watch->elapsed(), watch->scrobblePoint() );
        m_animation->setParent( watch );
        connect( m_animation, SIGNAL(pixmap( QPixmap )), SLOT(setPixmap( QPixmap )) );

        connect( watch, SIGNAL(paused( bool )), m_animation, SLOT(setPaused( bool )) );
        connect( watch, SIGNAL(timeout()), m_animation, SLOT(glow()) );

        setPixmap( m_animation->pixmap() );
        
        // this happens if we respool a track that we were observing already before
        if (watch->remaining() == 0)
            m_animation->glow( 0 );
    }
}


void
ScrobbleButton::setPixmap( const QPixmap& p )
{
    m_pixmap = p;
    update();
}


bool
ScrobbleButton::event( QEvent* e )
{
	switch ((int)e->type())
	{
		case QEvent::Enter:
		{
			if (m_tip->text().isEmpty()) break;

			QPoint p = mapToGlobal( QPoint(width() / 2, height()) );
			m_tip->move( p.x() - m_tip->rect().center().x(), p.y() );
			m_tip->show();
			break;
		}
		
		case QEvent::Leave:
			m_tip->hide();
			break;
	}
	
	return QAbstractButton::event( e );
}


void
ScrobbleButton::update()
{
	if (m_scrobble.isNull())
	{ 
		m_tip->clear();
		QAbstractButton::update();
		return; 
	}
	
	QString time = QTime().addSecs( m_scrobblePoint ).toString( "mm:ss" );
	QString text;
	
	//TODO also take into account scrobbler handshaken state?
	if (m_connectionMonitor->isUp())
		text = tr("Will scrobble at %1").arg( time );
	else
		text = tr( "Cannot reach Last.fm.<br>Will cache this scrobble at %1 for later submission." ).arg( time );

	m_tip->setText( text );
	m_tip->adjustSize();

	QAbstractButton::update();
}


void
ScrobbleButton::onChecked( bool b )
{
    if (b && m_animation && m_animation->done()) 
        m_animation->glow( 0 );
    else if (!b)
        m_pixmap = scrobbleButtonPixmap( 25 );
}

/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Christian Muehlhaeuser, Last.fm Ltd <chris@last.fm>                *
 *      Erik Jaelevik, Last.fm Ltd <erik@last.fm>                          *
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

#include "progressframe.h"
#include <QPainter>
#include <QPaintEvent>

QTimer* SecondsTimer::s_timer = 0;


ProgressFrame::ProgressFrame( QWidget *parent ) :
    QFrame( parent ),
    m_itemType( 0 ),
    m_progressEnabled( false ),
    m_clockEnabled( false ),
    m_reverse( false ),
    m_watch( NULL ),
    m_value( 0 )
{
    m_textPushTimer.setSingleShot( true );
    connect( &m_textPushTimer, SIGNAL( timeout() ), this, SLOT( popText() ) );
    m_clockPushTimer.setSingleShot( true );
    connect( &m_clockPushTimer, SIGNAL( timeout() ), this, SLOT( popClockText() ) );

    setMinimumHeight( 22 );
    setMinimumWidth( 22 );
    setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    setLineWidth( 1 );
}


void
ProgressFrame::clear()
{
    setText( "" );
    setClockText( "" );
    setEnabled( false );
    setValue( 0 );
    
    delete m_seconds_timer;
}


bool
ProgressFrame::isActive()
{
    return m_watch != NULL && !m_watch->isTimedOut();
}


void
ProgressFrame::setStopWatch( StopWatch* watch )
{
    m_watch = watch;
    prepareNewEndlessTimer();
    m_seconds_timer->setParent( watch );
    m_seconds_timer->start();

    setValue( m_watch ? m_watch->elapsed() : 0 );

    //m_progressEnabled = true;
    //m_clockEnabled = true;
    m_clockText = "";
}


void
ProgressFrame::prepareNewEndlessTimer()
{
    delete m_seconds_timer;
    m_seconds_timer = new SecondsTimer;
    connect( m_seconds_timer, SIGNAL(valueChanged( int )), SLOT(setValue( int )) );
}


void
ProgressFrame::startEndlessTimerIfNotAlreadyStarted()
{
    if (m_seconds_timer && !m_seconds_timer->isActive())
        m_seconds_timer->start();
}


void
ProgressFrame::setText( const QString& text )
{
    m_text = text;
    m_savedText = text;
    update();
}


void
ProgressFrame::pushText( const QString& text, int seconds )
{
    m_text = text;

    m_textPushTimer.stop();
    m_textPushTimer.start( seconds * 1000 );

    update();
}


void
ProgressFrame::popText()
{
    m_text = m_savedText;
    update();
}


void
ProgressFrame::setClockText( const QString& text )
{
    m_clockText = text;
    update();
}


void
ProgressFrame::pushClockText( const QString& text, int seconds )
{
    m_clockText = text;

    m_clockPushTimer.stop();
    m_clockPushTimer.start( seconds * 1000 );

    update();
}


void
ProgressFrame::popClockText()
{
    m_clockText = "";
    update();
}


void
ProgressFrame::setProgressEnabled( bool enabled )
{
    m_progressEnabled = enabled;
    update();
}


void
ProgressFrame::setClockEnabled( bool enabled )
{
    m_clockEnabled = enabled;
    update();
}


void
ProgressFrame::paintEvent( QPaintEvent* event )
{
    QRect rect = event->rect();
    rect.adjust( 8, 0, -8, 0 );

    QPainter painter( this );

    drawFrame( &painter );
    paintGradients( &painter );
    paintIcon( &painter, rect, icon() );

    QRect clockRect = paintClock( &painter, rect );
    paintText( &painter, rect, clockRect );
}


void
ProgressFrame::paintGradients( QPainter* painter, bool progressBar )
{
    int progressWidth = 0;

    if ( progressBar )
        progressWidth = paintProgressBar( painter, m_foregroundGradient );

    paintBackground( painter, progressWidth, m_backgroundGradient );
}


int
ProgressFrame::paintProgressBar( QPainter* painter, const QLinearGradient& grad )
{
    int w = 0;
    if ( m_watch != NULL && m_progressEnabled )
    {
        if ( m_watch->scrobblePoint() > 0 )
        {
            w = (int)( ( (float)m_value / (float)m_watch->scrobblePoint() ) * ( width() - 2 ) );
            if ( w > width() - 2 )
                w = width() - 2;
        }

        QRect r( 1, 1, w, height() - 2 );
        painter->fillRect( r, grad );
    }

    return w;
}


void
ProgressFrame::paintBackground( QPainter* painter, int xPos, const QLinearGradient& grad )
{
    QRect r( xPos + 1, 1, width() - 2 - xPos, height() - 2 );
    painter->fillRect( r, grad );
}


void
ProgressFrame::paintIcon( QPainter* painter, QRect rect, const QPixmap& icon )
{
    // icon if one is set
    if ( !icon.isNull() )
    {
        QRect r = rect;
        r.adjust( 0, ( rect.height() - icon.height() ) / 2, 0, 0 );
        r.setSize( icon.size() );
        painter->drawPixmap( r, icon );

        rect.adjust( icon.width() + 6, 1, 0, 0 );
    }
}


QRect
ProgressFrame::paintClock( QPainter* painter, QRect rect )
{
    // texts on top
    painter->setPen( Qt::black );
    painter->setBrush( Qt::black );

    // Draw time
    QString timeString;
    //bool endReached = ( value() >= m_watch->getTimeOut() ) &&
    //                  ( m_watch->getTimeOut() != -1 );

    int time = m_reverse ? m_watch->scrobblePoint() - value() : value();

    if ( !m_clockText.isEmpty() )
    {
        timeString = m_clockText;
    }
    else if ( m_clockEnabled && time >= 0 )
    {
        timeString = secondsToTimeString( time );
    }

    QRectF boundingRect;
    painter->drawText( rect, Qt::AlignRight | Qt::AlignVCenter, timeString, &boundingRect );

    return boundingRect.toRect();
}


QString
ProgressFrame::secondsToTimeString( int time )
{
    QString hrs  = QString::number( ( time / 3600 ) > 0 ? ( time / 3600 ) : 0 );
    QString mins = QString::number( ( ( time % 3600 ) / 60 ) > 0 ? ( ( time % 3600 ) / 60 ) : 0 );
    QString secs = QString::number( ( ( time % 3600 ) % 60 ) > 0 ? ( ( time % 3600 ) % 60 ) : 0 );

    QString timeString = QString( "%1%2%3:%4" )
        .arg( m_reverse ? "- " : "" )
        .arg( hrs == "0" ? "" : hrs + ":" )
        .arg( hrs == "0" ? mins : ( mins.length() < 2 ? "0" + mins : mins ) )
        .arg( secs.length() < 2 ? "0" + secs : secs );

    return timeString;
}


void
ProgressFrame::paintText( QPainter* painter, QRect rect, QRect clockRect )
{
    rect.adjust( 0, 0, -( clockRect.width() + 8 ), 0 );
    painter->drawText( rect, Qt::AlignLeft | Qt::AlignVCenter, text() );
}


void
ProgressFrame::setValue( int value )
{
    m_value = value;
    update();
}


void
ProgressFrame::mouseMoveEvent( QMouseEvent *event )
{
    if ( !( event->buttons() & Qt::LeftButton ) )
        return;

    QString anchor = text();
    if ( !anchor.isEmpty() )
    {
        QDrag *drag = new QDrag( this );

        QMimeData *mimeData = new QMimeData();
        mimeData->setData( "item/type", QByteArray::number( m_itemType ) );

        QMap<QString, QString> data = itemData();
        if ( data.count() )
        {
            for ( int i = 0; i < data.count(); i++ )
            {
                mimeData->setData( QString( "item/%1" ).arg( data.keys().at( i ) ), data.values().at( i ).toUtf8() );
            }
        }
        else
            return;

        mimeData->setText( mimeData->data( "item/artist" ) + " - " + mimeData->data( "item/track" ) );

        QPainter painter;
        QPixmap pixmap( painter.fontMetrics().width( anchor ) + 16, painter.fontMetrics().height() + 4 );
        QRect rect( 0, 0, pixmap.width() - 1, pixmap.height() - 1 );

        painter.begin( &pixmap );
        painter.setBackgroundMode( Qt::OpaqueMode );

        painter.setBrush( Qt::white );
        painter.setPen( Qt::black );
        painter.drawRect( rect );

        painter.setPen( Qt::black );
        painter.drawText( rect, Qt::AlignCenter, anchor );
        painter.end();

        drag->setMimeData( mimeData );
        drag->setPixmap( pixmap );

        Qt::DropAction dropAction = drag->start( Qt::CopyAction );

        Q_UNUSED( dropAction )
    }
}

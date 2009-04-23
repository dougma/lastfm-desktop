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

#include "TrackProgressFrame.h"
#include "UglySettings.h"
#include <lastfm/Scrobble>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>


TrackProgressFrame::TrackProgressFrame( QWidget *parent ) :
        ProgressFrame( parent ),
        m_source( TrackInfo::Unknown ),
        m_trackLength( -1 ),
        m_scrobblingEnabled( true ),
        m_scrobbled( false ),
        m_trackIsScrobblable( false ),
        m_clockRect( QRect( -1, -1, -1, -1 ) ),
        m_clockShowsTrackTime( true )
{
    setMouseTracking( true );

    m_clockShowsTrackTime = The::settings().currentUser().trackFrameClockMode();
}


void
TrackProgressFrame::setTrack( TrackInfo& track )
{
    m_trackLength = track.duration();
    m_scrobbled = false;
    m_trackIsScrobblable = Scrobble( track ).isValid();
    m_source = track.source();

    setText( textForScrobblableStatus( track ) );

    if ( m_source == TrackInfo::Player )
    {
        // Force this off if we're doing local playback as we have no track time
        m_clockShowsTrackTime = false;
    }
    else
    {
        // Restore it to what it was previously
        m_clockShowsTrackTime = The::settings().currentUser().trackFrameClockMode();
    }

    QMap<QString, QString> itemdata;
    itemdata.insert( "artist", track.artist() );
    itemdata.insert( "track", track.track() );
    setItemData( itemdata );
}


QString
TrackProgressFrame::textForScrobblableStatus( TrackInfo& track )
{
    QString text = tr( "Can't scrobble: %1" );
    
    Scrobble::Invalidity invalidity;
    if (Scrobble( track ).isValid( &invalidity ))
    {
//TODO        if (track.isPowerPlay())
//TODO            return tr( "%1 (Brought to you by %2)" ).arg( track.toString() ).arg( track.powerPlayLabel() );

        return track.toString();
    }
    else switch (invalidity)
    {
        case Scrobble::ArtistNameMissing:
        case Scrobble::TrackNameMissing:
            return text.arg( tr( "artist or title missing from ID3 tag" ) );

        case Scrobble::TooShort: return text.arg( tr( "track too short" ) );
        case Scrobble::ForbiddenPath: return tr( "Won't scrobble: track is in directory set to not scrobble" );
        case Scrobble::ArtistInvalid: return text.arg( tr( "invalid artist name" ) );

        case Scrobble::NoTimestamp:
        case Scrobble::FromTheFuture: 
        case Scrobble::FromTheDistantPast:
            return text.arg( tr( "invalid start time" ) );
    }

    return "";
}


void
TrackProgressFrame::setScrobblingEnabled( bool en )
{
    m_scrobblingEnabled = en;
    update();
}


void
TrackProgressFrame::paintEvent( QPaintEvent * event )
{
    bool paintProgress = isScrobblable() && value() < watchTimeOut();

    QRect rect = event->rect();
    rect.adjust( 8, 0, -8, 0 );

    QPainter painter( this );

    drawFrame( &painter );
    paintGradients( &painter, paintProgress );

    if ( isScrobblable() && ( value() >= watchTimeOut() ) )
            paintProgressBar( &painter, m_scrobbledGradient );

    paintIcon( &painter, rect, icon() );

    // This calls the overridden version
    m_clockRect = paintClock( &painter, rect );
    paintText( &painter, rect, m_clockRect );
}


QRect
TrackProgressFrame::paintClock( QPainter* painter, QRect rect )
{
    // texts on top
    painter->setPen( Qt::black );
    painter->setBrush( Qt::black );

    // Draw time
    QString timeString;

    if ( !clockText().isEmpty() )
    {
        // Clock text always overrides time string
        timeString = clockText();
    }
    else if ( clockEnabled() )
    {
        //                Radio     Local
        // Track time     yes       no
        // Scrobble time  yes       yes

        Q_ASSERT( m_source != TrackInfo::Unknown );

        if ( m_clockShowsTrackTime )
        {
            // Display track timer

            if ( m_source == TrackInfo::Player )
            {
                // Have no track time info for local scrobbles
                timeString = "";
            }
            else
            {
                // Radio has track length
                int displayTime = reverse() ? m_trackLength - value() : value();
                timeString = secondsToTimeString( displayTime );
            }
        }
        else
        {
            // Display scrobble timer

            bool scrobblingOff = !isScrobblable();
            if ( scrobblingOff && !m_scrobbled )
            {
                timeString = tr( "won't scrobble", "Needs to be very short, displayed in track progress bar" );
            }
            else
            {
                bool scrobbled = m_watch->isTimedOut();
                if ( scrobbled )
                {
                    timeString = tr( "scrobbled", "Needs to be very short, displayed in track progress bar" );
                    m_scrobbled = true;
                }
                else
                {
                    int displayTime = reverse() ? watchTimeOut() - value() : value();
                    timeString = secondsToTimeString( displayTime );
                }
            }
        }

    }

    QRectF boundingRect;
    painter->drawText( rect, Qt::AlignRight | Qt::AlignVCenter, timeString, &boundingRect );

    return boundingRect.toRect();
}


void
TrackProgressFrame::mousePressEvent( QMouseEvent* event )
{
    if ( m_source != TrackInfo::LastFmRadio )
        return;

    if ( event->button() == Qt::LeftButton )
    {
        if ( m_clockRect.contains( event->pos() ) )
        {
            m_clockShowsTrackTime = !m_clockShowsTrackTime;
            update();

            The::settings().currentUser().setTrackFrameClockMode( m_clockShowsTrackTime );
        }
    }
}


void
TrackProgressFrame::mouseMoveEvent( QMouseEvent* event )
{
    if ( m_source != TrackInfo::LastFmRadio )
        return;

    if ( m_clockRect.contains( event->pos() ) )
        QToolTip::showText( mapToGlobal( QPoint( width() - 40, height() / 2 ) ),
                            tr( "Click to toggle between track time and scrobble time" ), this );
    else
        QToolTip::hideText();
}


bool
TrackProgressFrame::isScrobblable() const
{
    return m_scrobblingEnabled && m_trackIsScrobblable;
}

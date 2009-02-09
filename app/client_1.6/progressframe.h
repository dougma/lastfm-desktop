/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Christian Muehlhaeuser, Last.fm Ltd <chris@last.fm>                *
 *      Erik Jalevik, Last.fm Ltd <erik@last.fm>                           *
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

#ifndef PROGRESSFRAME_H
#define PROGRESSFRAME_H

#include "StopWatch.h"
#include "lib/lastfm/types/Track.h"
#include <QFrame>
#include <QPointer>
#include <QTimer>
class QPainter;
class SecondsTimer;


class ProgressFrame : public QFrame
{
    Q_OBJECT

    public:
        ProgressFrame( QWidget *parent = 0 );

        void clear();
        bool isActive();
        int value() { return m_value; }

        // The stopwatch whose time to reflect. The StopWatch must have a
        // timeout for reverse display to work.
        void setStopWatch( StopWatch* watch );

        bool reverse() { return m_reverse; }
        void setReverse( bool reverse ) { m_reverse = reverse; }

        // The main text
        QString text() { return m_text; }
        void setText( const QString& text );

        // Temporary message that should only be displayed for a few seconds
        void pushText( const QString& text, int seconds = 3 );

        // Allows setting a text string where the clock normally is
        QString clockText() { return m_clockText; }
        void setClockText( const QString& text );

        // Temporary message that should only be displayed for a few seconds
        void pushClockText( const QString& text, int seconds = 3 );

        // Icon displayed to the left of the text
        QPixmap icon() { return m_pixmap; }
        void setIcon( QPixmap pixmap ) { m_pixmap = pixmap; }

        // Background and progress bar gradient
        void setBackgroundGradient( const QLinearGradient& bg ) { m_backgroundGradient = bg; }
        void setForegroundGradient( const QLinearGradient& fg ) { m_foregroundGradient = fg; }

        // No progress will be shown is this is off
        bool progressEnabled() { return m_progressEnabled; }
        void setProgressEnabled( bool enabled );

        // No clock will be shown if this is off
        bool clockEnabled() { return m_clockEnabled; }
        void setClockEnabled( bool enabled );

        void setEnabled( bool enabled )
        {
            setProgressEnabled( enabled );
            setClockEnabled( enabled );
        }

        int watchTimeOut() { return m_watch != NULL ? m_watch->scrobblePoint() : -1 ; }

        // Drag and drop functions
        QMap<QString, QString> itemData() { return m_itemData; }
        void setItemData( QMap<QString, QString> data ) { m_itemData = data; }
        int itemType() { return m_itemType; }
        void setItemType( int type ) { m_itemType = type; }

    public slots:

    protected:

        void
        mouseMoveEvent( QMouseEvent *event );

        virtual int
        paintProgressBar( QPainter* painter, const QLinearGradient& grad );

        virtual void
        paintBackground( QPainter* painter, int xPos, const QLinearGradient& grad );

        virtual void
        paintIcon( QPainter* painter, QRect rect, const QPixmap& icon );

        virtual QRect
        paintClock( QPainter* painter, QRect rect );

        virtual void
        paintText( QPainter* painter, QRect rect, QRect clockRect );

        virtual void
        paintGradients( QPainter* painter, bool progressBar = true );

        QString
        secondsToTimeString( int time );

    private:

        void paintEvent( QPaintEvent* event );

        QPixmap m_pixmap;

        QString m_text;
        QString m_savedText;

        QString m_clockText;

        QLinearGradient m_backgroundGradient;
        QLinearGradient m_foregroundGradient;

        int m_itemType;
        QMap<QString, QString> m_itemData;

        bool m_progressEnabled;
        bool m_clockEnabled;

        bool m_reverse;

        QTimer m_textPushTimer;
        QTimer m_clockPushTimer;

        StopWatch* m_watch;
        QPointer<SecondsTimer> m_seconds_timer;
        int m_value;

    private slots:

        void setValue( int secs );

        void popText();
        void popClockText();

};


#include <cmath>
class SecondsTimer : public QTimer
{
    Q_OBJECT
    QTime elapsed;
    
public:
    SecondsTimer()
    {
        elapsed.start();
        setInterval( 1000 );
        connect( this, SIGNAL(timeout()), SLOT(onTimeout()) );
        start();
    }
    
private slots:
    void onTimeout()
    {
        emit valueChanged( (int) ::round( float(elapsed.elapsed()) / 1000.0f ) );
    }
    
signals:
    void valueChanged( int );
};

#endif

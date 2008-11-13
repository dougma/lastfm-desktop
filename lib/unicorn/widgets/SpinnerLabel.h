/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *    This program is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/
#ifndef SPINNER_LABEL_H
#define SPINNER_LABEL_H

#include <QEvent>
#include <QLabel> //TODO implementation
#include <QMovie> //TODO implementation
#include <QDebug>
 
class SpinnerLabel : public QLabel
{
    virtual bool event( QEvent* e )
    {
        switch ((int)e->type())
        {
        case QEvent::Hide:
            m_movie->stop();
            break;
        case QEvent::Show:
            m_movie->start();
            break;
        }

        return QLabel::event( e );
    }

    QMovie* m_movie;

public:
    SpinnerLabel( QWidget* parent = 0 ) : QLabel( parent )
    {
        setMovie( m_movie = new QMovie( ":/lastfm/spinner.mng" ) );
        m_movie->setParent( this );
        //qt fucking sucks
        setFixedSize( 25, 18 );
    }
};

#endif //SPINNER_LABEL_H

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

#include "TagBuckets.h"
#include "lib/lastfm/types/Track.h"
#include <QtGui>


TagBuckets::TagBuckets( const Track& t )
{
    QVBoxLayout* v = new QVBoxLayout( this );
    v->addWidget( new TagBucket( t.album() ) );
    v->addWidget( new TagBucket( t.artist() ) );
    v->addWidget( new TagBucket( t.title() ) );
    v->setMargin( 0 );
}


TagBucket::TagBucket( const QString& title )
{
    QVBoxLayout* v = new QVBoxLayout( this );
    v->addWidget( new QLabel( title ) );
    v->addWidget( new QTextEdit );
    v->setSpacing( 0 );
    v->setMargin( 0 );
}

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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02111-1307, USA.          *
 ***************************************************************************/
 
#ifndef FINGERPRINTER_SETTINGS_H
#define FINGERPRINTER_SETTINGS_H

#include "lib/unicorn/UnicornSettings.h"


/** @author <petgru@last.fm> 
  * @author <adam@last.fm>
  */
class Settings : public unicorn::Settings
{
    void setValue( QString key, QVariant v )
    {
        QSettings s;
        s.setValue( key, v );
    }

    QVariant value( QString key, QVariant default_value = QVariant() ) const
    {
        QSettings s;        
        return s.value( key, default_value );
    }
    
    template <class T> T value( QString key, QVariant default_value = QVariant() ) const
    {
        return value( key, default_value ).value<T>();
    }
    
public:
    // WINDOW STUFF
    void setSize( QSize size ) { setValue ( "window/size", size ); }
    void setPosition( QPoint pos ) { setValue ( "window/position", pos ); }
    
    QSize size() { return value<QSize>  ( "window/size", QSize(450, 700) ); }
    QPoint position() { return value<QPoint> ( "window/position", QPoint(200, 200) ); }
};

#endif

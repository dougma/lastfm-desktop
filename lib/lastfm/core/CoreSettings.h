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

#ifndef CORE_SETTINGS_H
#define CORE_SETTINGS_H

#include "lib/lastfm/core/CoreLocale.h"
#include <QSettings>


/** Settings that are global to all Last.fm libraries */
class CoreSettings : public QSettings
{
public:
    /** applicationName allows you to create arbituary settings for other 
      * Last.fm applications, but don't use it pls */
	CoreSettings( const QString& applicationName = "" ) : QSettings( organizationName(), applicationName )
	{}
    
    static const char* organizationName()
    {
    #ifdef NDEBUG
        return "Last.fm";
    #else
        return "Last.am";
    #endif
    }
    
    static const char* organizationDomain()
    {
    #ifdef NDEBUG
        return "last.fm";
    #else
        return "last.am";
    #endif
    }

	CoreLocale locale() const
	{
		QVariant const v = value( "Locale" );
		return v.isValid() ? QLocale( QLocale::Language(v.toInt()) ) : CoreLocale::system();
	}
};

#endif

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

#ifndef RADIO_STATION_H
#define RADIO_STATION_H

#include "lib/DllExportMacro.h"
#include <QString>
#include <QMetaType>


/** FIXME I think these should be static constructors, enum types look contrived in code that uses them 
  */
class RADIO_DLLEXPORT RadioStation
{
public:
	enum Type { SimilarArtist = 0, Recommendation, Library, Neighbourhood, Loved, Tag, Url };
	
    RadioStation( QString s, Type t = Url, QString title = "" ) : m_station( s ), m_type( t ), m_title( title ){}
	
	operator const QString() const
	{
		switch ( m_type ) 
		{
			case SimilarArtist:	return "lastfm://artist/" + m_station + "/similarartists";
			case Recommendation: return "lastfm://user/" + m_station + "/recommended";
			case Library: return "lastfm://user/" + m_station + "/personal";
			case Neighbourhood: return "lastfm://user/" + m_station + "/neighbours";
			case Loved: return "lastfm://user/" + m_station + "/loved";
			case Tag: return "lastfm://globaltags/" + m_station;
				
			case Url: return m_station;
			
			default: Q_ASSERT( !"I can't generate station url string for an unknown station type:" + m_type ); return "";
		}
	}
	
	const QString& title() const{ return m_title; }
	
private:
	QString m_station;
	Type m_type;
	QString m_title;
};

#endif

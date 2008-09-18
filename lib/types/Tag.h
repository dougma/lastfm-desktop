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

#ifndef TAG_H
#define TAG_H

#include "lib/DllExportMacro.h"
#include "lib/core/WeightedStringList.h"
#include <QString>
#include <QUrl>

class TYPES_DLLEXPORT Tag
{
	QString m_name;
	
public:
	Tag( const QString& name ) : m_name( name )
	{}
	
	operator QString() const { return m_name; }
	
	/** the global tag page at www.last.fm */
	QUrl url() const;
	
	/** the tag page for user @p user at www.last.fm */
	QUrl url( const class User& user ) const;
	
	class WsReply* search() const;
	static QStringList search( WsReply* );
    
    /** you can in fact pass any *.getTopTags result to this */
    static WeightedStringList list( WsReply* );
};

#endif

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

#ifndef UNICORN_USER_H
#define UNICORN_USER_H

#include <QString>
#include "lib/DllExportMacro.h"
#include "lib/core/WeightedStringList.h"
#include "lib/ws/WsReply.h" //convenience


typedef QList<class User> UserList;

class TYPES_DLLEXPORT User
{
    QString m_name;
	
public:
    User( const QString& username ) : m_name( username ), m_match( -1.0f )
    {}

    operator QString() const { return m_name; }

    WsReply* getFriends();
    static UserList getFriends( WsReply* );
	
	/** you can only get information about the autheticated user */
	static WsReply* getInfo();

	WsReply* getTopTags();
	static WeightedStringList getTopTags( WsReply* );
	
	WsReply* getNeighbours();
	static UserList getNeighbours( WsReply* );
	
	QUrl smallImageUrl() const{ return m_smallImage; }
	QUrl mediumImageUrl() const{ return m_mediumImage; }
	QUrl largeImageUrl() const{ return m_largeImage; }
	
	/** Returns the match between the logged in user
		and the user which this object represents
		( if < 0.0f then not set ) */
	float match() const{ return m_match; }
	
private:
	QUrl m_smallImage;
	QUrl m_mediumImage;
	QUrl m_largeImage;
	
	float m_match;
};

#endif

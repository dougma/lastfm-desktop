/***************************************************************************
 *   Copyright (C) 2004 - 2007 by                                          *
 *      Gareth Simpson <iscrobbler@xurble.org>                             *
 *      Christian Muehlhaeuser, Last.fm Ltd. <chris@last.fm>               *
 *      Erik Jaelevik, Last.fm Ltd. <erik@last.fm>                         *
 *      Max Howell, Last.fm Ltd. <max@last.fm>                             *
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

#ifdef WIN32
    #include <Windows.h>
#endif

#include <fstream>
#include <iostream>
#include <string>

std::ofstream gLogFile;

void Log( const std::string& msg )
{
	if ( gLogFile.is_open() )
		gLogFile << msg << std::endl;
}


void Log( std::wstring& msg )
{
#ifdef WIN32
	if ( gLogFile.is_open() )
	{
		// first call works out required buffer length
		int recLen = WideCharToMultiByte( CP_ACP, 0, msg.c_str(), (int)(msg.size()), NULL, NULL, NULL, NULL );
        
		char* buffer = new char[recLen + 1];
		memset(buffer,0,recLen+1);
        
		// second call actually converts
		WideCharToMultiByte(CP_ACP,0,msg.c_str(),(int)(msg.size()),buffer,recLen,NULL,NULL);
        
		std::string s = buffer;
	    
		gLogFile << s << std::endl;
	}
#endif
}


void Log(const std::wstring& msg)
{
	Log( const_cast<std::wstring&>(msg) );
}

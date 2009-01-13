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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#ifndef WS_AUTOPROXY_H
#define WS_AUTOPROXY_H

#include <QNetworkProxy>
#include <windows.h>
#include <winhttp.h>


/** @brief simple wrapper to do per url automatic proxy detection 
  * @author <doug@last.fm>
  */
class Pac
{    
	HINTERNET m_hSession;
    bool m_bFailed;

public:
	Pac();
	~Pac();

	QNetworkProxy resolve( const class QNetworkRequest& url, const wchar_t* pacUrl );

    void resetFailedState() { m_bFailed = false; }

private:
    Pac( const Pac& ); //undefined
    Pac operator=( const Pac& ); //undefined
};

#endif 
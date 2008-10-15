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
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
***************************************************************************/

#ifndef LASTFM_WS_ACCESS_MANAGERINIT_H
#define LASTFM_WS_ACCESS_MANAGERINIT_H

#include <lastfm/DllExportMacro.h>

class LASTFM_WS_DLLEXPORT WsAccessManagerInit
{
public:
#ifdef WIN32
    // special setup requirements for windows

    WsAccessManagerInit();
    ~WsAccessManagerInit();

    class ComSetup *m_com;

#else

    // nothing to do on other platforms

    WsAccessManagerInit() {}
    ~WsAccessManagerInit() {}

#endif
};

#endif
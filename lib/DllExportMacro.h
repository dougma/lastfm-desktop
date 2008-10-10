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

#ifndef DLL_EXPORT_MACRO_H
#define DLL_EXPORT_MACRO_H

/** Exports symbols when compiled as part of the lib
  * Imports when included from some other target */
#if defined(_WIN32) || defined(WIN32)
	#ifdef _UNICORN_DLLEXPORT
	    #define UNICORN_DLLEXPORT __declspec(dllexport)
	#else
	    #define UNICORN_DLLEXPORT __declspec(dllimport)
	#endif
#else
    #define UNICORN_DLLEXPORT
#endif

#endif

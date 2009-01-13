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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include "app/moose.h"
#include <QFileInfo>


/** you need to link to unicorn to use the non-inlined functions. 
  * <max@last.fm> 
  */
namespace twiddly
{
	static inline const char* id() { return "Twiddly-05F67299-64CC-4775-A10B-0FBF41B6C4D0"; }
    /** passed to QCoreApplication::setApplicationName() */
    static inline const char* applicationName() { return "Twiddly"; }

    /** returns the path to the executable, if we know it, or a guess, if we
      * don't, we generally know the path on eg. Windows. Or if the client has 
      * been run at least once. */
    static inline QString path()
    {
    #ifdef __APPLE__
        return QFileInfo( moose::dir().filePath( "../Resources/twiddly" ) ).absoluteFilePath();
    #endif
    #ifdef WIN32
        return moose::dir().filePath( "twiddly.exe" );
    #endif
    }
}

/***************************************************************************
 *   Copyright 2008 Last.fm Ltd. <client@last.fm>                          *
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

#include "logger.h"
#include <sys/stat.h>


/** @author Max Howell 
  * @brief Used by Twiddly and iTunesPlugin
  */
namespace common
{
    #if WIN32
        #define STRING wstring
        #define STAT _stat
        #define stat _wstat
        #define COMMON_LOGL LOGWL
    #else
        #define STAT stat
        #define STRING string
        #define COMMON_LOGL LOGL
    #endif

    static time_t
    fileCreationTime( const std::STRING& path )
    {
        struct STAT st;
        if (stat( path.c_str(), &st ) != 0)
        {
            COMMON_LOGL( 3, "Couldn't stat: " << path );
            return 0;
        }
        else
            return st.st_ctime ? st.st_ctime : st.st_mtime;
    }

    #undef STAT
    #undef stat
    #undef STRING
    #undef COMMON_LOGL

#ifdef QT_CORE_LIB
    static inline time_t
    fileCreationTime( const QString& path )
    {
    #ifdef Q_OS_WIN32
        return fileCreationTime( path.toStdWString() );
    #else
        return fileCreationTime( path.toStdString() );
    #endif
    }
#endif
}

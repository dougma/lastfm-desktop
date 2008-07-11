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

#ifndef MOOSE_COMMON_H
#define MOOSE_COMMON_H

#include "lib/DllExportMacro.h"
#include <QString>


namespace Moose
{
    enum StartNewInstanceBehaviour
    {
        StartNewInstance,
        DontStartNewInstance
    };

    /** @returns true if the client is already running */
    MOOSE_DLLEXPORT bool isAlreadyRunning();

    /** Sends the command to the running client instance, or starts the 
      * client then sends it, NOTE we don't currently support spaces in 
      * @p command */
    MOOSE_DLLEXPORT bool sendToInstance( const QString& command,
                                         StartNewInstanceBehaviour = DontStartNewInstance );

    /** @returns information about the session. More or less the 
      * information you get from MooseUtils and MoseSettings */
    //TODO move to SendLongDialog or whatever
    MOOSE_DLLEXPORT QString sessionInformation();

}

#endif

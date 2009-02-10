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

#include <string.h>
#include "../localresolver/TrackResolver.h"
#include "../localresolver/LocalRqlPlugin.h"
#include "../localresolver/TrackTagUpdater.h"


/** always exporting, never importing  **/
#if defined(_WIN32) || defined(WIN32)
    #define RESOLVER_DLLEXPORT __declspec(dllexport)
#else
    #define RESOLVER_DLLEXPORT
#endif

TrackTagUpdater *gTrackTagUpdater = 0;



extern "C" {

RESOLVER_DLLEXPORT
void *
lastfm_getService(const char *service)
{
    if (0 == gTrackTagUpdater) {
        gTrackTagUpdater = TrackTagUpdater::create(
            "http://musiclookup.last.fm/trackresolve",
            100,        // number of days track tags are good 
            5);         // 5 minute delay between web requests
    }

    if (0 == strcmp("TrackResolver", service)) {
        return static_cast<ITrackResolverPlugin*>(new TrackResolver());
    }
    if (0 == strcmp("LocalRql", service)) {
        return static_cast<ILocalRqlPlugin*>(new LocalRqlPlugin());
    }
    return 0;
}

}


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

#ifndef TUNER_H
#define TUNER_H

#include "RadioStation.h"
#include "lib/DllExportMacro.h"
#include "lib/types/Track.h"
#include <QList>
#include <QUrl>


class RADIO_DLLEXPORT Tuner
{
public:
    Tuner()
    {}

        /** If you aren't a Unicorn::Application, you won't get Radio FIXME lame */
    explicit Tuner( const RadioStation& );

    /** returns next 5 tracks */
    QList<Track> fetchNextPlaylist();
	const QString& stationName() const{ return m_stationName; }

private:
    QList<Track> getPlaylist();
	QString m_stationName;
};

#endif

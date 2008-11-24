/***************************************************************************
 *   Copyright 2007-2008 Last.fm Ltd.                                      *
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

#include "LocalRqlPlugin.h"
#include "TagUpdater.h"
#include "RqlQuery.h"


LocalRqlPlugin::LocalRqlPlugin()
: m_tagUpdater(0)
, m_query(0)
{
}

void 
LocalRqlPlugin::init()
{
    m_tagUpdater = new TagUpdater();
// todo: replace this
//    m_tagUpdater->start();
}

ILocalRqlPull* 
LocalRqlPlugin::play(const char *rql)
{
    if (!m_query)
        m_query = new RqlQuery();
    
    QSet<uint> results = m_query->doQuery(rql);

    int i = results.size();

    return 0;
}

void 
LocalRqlPlugin::finished(ILocalRqlPull* todo)
{
    delete m_tagUpdater;
    delete m_query;
}

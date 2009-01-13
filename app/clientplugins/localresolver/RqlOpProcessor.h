/***************************************************************************
 *   Copyright 2007-2009 Last.fm Ltd.                                      *
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

#ifndef RQL_OP_PROCESSOR_H
#define RQL_OP_PROCESSOR_H

#include "RqlOp.h"
#include "ResultSet.h"
#include <QList>

class RqlOpProcessor
{
    QList<RqlOp>::iterator m_it, m_end;
    class LocalCollection& m_collection;
    class SimilarArtists& m_similarArtists;

    RqlOpProcessor(QList<RqlOp> &ops, LocalCollection& collection, SimilarArtists& similarArtists);
    void next();
    ResultSet process();
    ResultSet unsupported();
    ResultSet globalTag();
    ResultSet userTag();
    ResultSet artist();
    ResultSet similarArtist();

    void normalise(float weight, ResultSet& rs);

public:
    static ResultSet process(QList<RqlOp> &ops, LocalCollection& collection, SimilarArtists& similarArtists);

};

#endif
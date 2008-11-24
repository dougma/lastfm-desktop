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

#include "RqlQuery.h"
#include "LocalCollection.h"
#include "RqlOpProcessor.h"
#include "rqlParser/parser.h"

using namespace std;
using namespace fm::last::query_parser;


RqlOp root2op( const querynode_data& node )
{
   RqlOp op;
   op.isRoot = true;
   op.name = node.name;
   op.type = node.type;
   op.weight = node.weight;

   return op;
}


RqlOp leaf2op( const querynode_data& node )
{
   RqlOp op;
   op.isRoot = false;

   if ( node.ID < 0 )
      op.name = node.name;
   else
   {
      ostringstream oss;
      oss << '<' << node.ID << '>';
      op.name = oss.str();
   }
   op.type = node.type;
   op.weight = node.weight;

   return op;
}



RqlQuery::RqlQuery()
:m_collection(0)
{
}

RqlQuery::~RqlQuery()
{
    delete m_collection;
}

QSet<uint>
RqlQuery::doQuery(const char *rql)
{
    if (!m_collection)
        m_collection = LocalCollection::create("RqlQuery");

    parser p;
    if (p.parse(string(rql))) {
        vector<RqlOp> ops;
        p.getOperations<RqlOp>(ops, &root2op, &leaf2op);
        return RqlOpProcessor::process(ops, *m_collection, m_similarArtists);
    }

    return QSet<uint>();
}

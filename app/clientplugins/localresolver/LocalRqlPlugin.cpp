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

#include <QDebug>
#include "LocalRqlPlugin.h"
#include "TagUpdater.h"
#include "RqlQuery.h"

#include "RqlOpProcessor.h"
#include "rqlParser/parser.h"
#include <boost/bind.hpp>

#include <vector>

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



LocalRqlPlugin::LocalRqlPlugin()
: m_tagUpdater(0)
, m_localCollection(0)
{
}

void 
LocalRqlPlugin::init()
{
    m_localCollection = LocalCollection::create("RqlQuery");
    m_tagUpdater = new TagUpdater();
// todo: replace this
//    m_tagUpdater->start();
}

void
LocalRqlPlugin::parse(const char *rql, ILocalRqlParseCallback *cb)
{
    Q_ASSERT(rql && cb);

    try {
        string srql(rql);   // needs to exist for the life of p
        parser p;
        if (p.parse(srql)) {
            QList<RqlOp> ops;

            p.getOperations<RqlOp>(
                boost::bind(&QList<RqlOp>::push_back, boost::ref(ops), _1),
                &root2op, 
                &leaf2op);
            
            cb->parseOk(
                new RqlQuery(
                    *m_localCollection,
                    RqlOpProcessor::process(ops, *m_localCollection, m_sa) ) );
        } else {
            cb->parseFail(
                p.getErrorLineNumber(),
                p.getErrorLine().data(),
                p.getErrorOffset() );
        }
    } 
    catch (...) {
        qCritical() << "unexpected exception LocalRqlPlugin::parse";
    }
}

void 
LocalRqlPlugin::finished()
{
    delete m_tagUpdater;
    delete m_localCollection;
}

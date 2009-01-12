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

#include "RqlStationTemplate.h"


RqlStationTemplate::RqlStationTemplate( FieldNode* f )
                   :m_rootNode( f )
{
}


RqlStationTemplate::Iterator::Iterator( QueryNode* t )
{
    m_rootNode = t;
    reset();
}


void 
RqlStationTemplate::Iterator::reset()
{
    m_traversalStack.clear();
    m_firstTime = true;
    m_curNode = m_rootNode;
}


QueryNode*
RqlStationTemplate::Iterator::next()
{
    QueryNode *n;
    if( m_firstTime )
    {
        n = m_rootNode;
        m_firstTime = false;
    }
    else
    {
        n=m_curNode;
        n=n->rp;
    }
    
    do
    {
        while( n != NULL )
        { //push all left children on the stack
            m_traversalStack.push(n);
            n=n->lp;
        }
        if( !m_traversalStack.isEmpty() )
        { //if the traversal stack is not empty, visit the top item
            n=m_traversalStack.pop();
            m_curNode=n;
            return m_curNode;
        }
    }
    while( !m_traversalStack.isEmpty());
    m_curNode=NULL; //the traversal is done
    return m_curNode;
}


FieldNode* 
RqlStationTemplate::Iterator::nextField()
{
    QueryNode* n;
    for( n = next(); n && ( n->lp || n->rp ) ; n = next() );
    return (FieldNode*)n;
}
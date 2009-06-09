/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QString>
#include <QPair>
#include <QStack>
#include <QRegExp>
#include "radio/buckets/Seed.h"


class QueryNode
{ 
public: 
    QueryNode( QString rql ): m_rql(rql), lp( 0 ), rp( 0 ){}; 
    virtual ~QueryNode(){};
    
    QString m_rql; 
    virtual QString rql() const = 0;
    QueryNode* lp;
    QueryNode* rp;
};

class FieldNode : public QueryNode
{
public:
    FieldNode( QString rql, QString displayName, QString value = QString() )
    :QueryNode(rql), m_displayName(displayName), m_value(value)
    {
        if( rql.contains( QRegExp( "recs?|user|library|loved?" )))
            m_seedType = Seed::UserType;
        
        else if( rql.contains( QRegExp( "simart|art|" )))
            m_seedType = Seed::ArtistType;
        
        else if( rql.contains( QRegExp( "tag" )))
            m_seedType = Seed::TagType;
    }
    
    QString rql() const{ return m_rql + "\"" + value() + "\"" ; }
    
    void setValue( QString value ){ m_value = value; }
    QString value() const{ return m_value; }
    
    Seed::Type seedType() const{ return m_seedType; }
    
    QString displayName() const{ return m_displayName; }
    
private:
    QString m_displayName;
    QString m_value;
    Seed::Type m_seedType;
};

class OperatorNode : public QueryNode
{
public:
    OperatorNode( QString rql, QueryNode* operand1, QueryNode* operand2 ): QueryNode( rql )
    { 
        lp = operand1; 
        rp = operand2;
    }
    
    QString rql() const
    {
        QString out;
        out = lp->rql() + 
        " " +
        m_rql +
        " " +
        rp->rql();
        return out;
    }
    
};


class RqlStationTemplate
{
public:    
    class Iterator
    {
    public:
        Iterator(class QueryNode *t);
        void reset();
        
        class QueryNode* next();
        class FieldNode* nextField();
        
    private:
        QStack<QueryNode *> m_traversalStack; // maintains traversal state information
        QueryNode* m_rootNode;
        QueryNode* m_curNode;
        bool m_firstTime;
    };

    RqlStationTemplate( class FieldNode* f = 0 );
    
    void setTitle( const QString& t ){ m_title = t; }
    QString title() const{ return m_title; }
    
    void setDescription( const QString& d ){ m_description = d; }
    
    RqlStationTemplate& and( class FieldNode* const f, int min = 1, int max = 1 )
    {
        OperatorNode* op = new OperatorNode( "and", m_rootNode, f );
        m_rootNode = op;
        return *this;
    }
    
    RqlStationTemplate& or( class FieldNode* const f )
    {
        OperatorNode* op = new OperatorNode( "or", m_rootNode, f );
        m_rootNode = op;
        return *this;
    }
    
    RqlStationTemplate& not( class FieldNode* const f )
    {
        OperatorNode* op = new OperatorNode( "not", m_rootNode, f );
        m_rootNode = op;
        return *this;
    }


    QString rql() const
    {
        return m_rootNode->rql();
    }
    
    Iterator iterator()
    {
        return Iterator( m_rootNode );
    }
    
        
private:
    QString m_title;
    QString m_description;
    
    QueryNode* m_rootNode;
    
};


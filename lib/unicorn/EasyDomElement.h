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

#include "UnicornException.h"
#include <QDomElement>
#include <QList>

/** @author <max@last.fm>
  * @brief facade pattern for QDomElement, throwing exceptions in situations that we must handle
  */
class EasyDomElement
{
    QDomElement e;

public:
    class Exception : public UnicornException
    {
        friend EasyDomElement;

        Exception( QString s ) : UnicornException( s )
        {}

    public:
        static Exception nullNode() { return Exception( "Expected node absent." ); }
        static Exception emptyTextNode( QString name ) { return Exception( "Unexpected empty text node: " + name ); }
    };

    EasyDomElement( const QDomElement& x ) : e( x )
    {
        if (e.isNull()) throw Exception::nullNode();
    }

    EasyDomElement operator[]( const QString& name )
    {
        return EasyDomElement( e.firstChildElement( name ) );
    }

    /** use in all cases where empty would be an error */
    QString nonEmptyText() const 
    {
        QString const s = e.text();
        if (s.isEmpty())
            throw Exception::emptyTextNode( e.tagName() );
        return s;
    }

    QString text() const { return e.text(); }

    QList<EasyDomElement> children( const QString& named )
    {
        QList<EasyDomElement> elements;
        QDomNodeList nodes = e.elementsByTagName( named );
        for (int x = 0; x < nodes.count(); ++x)
            elements += EasyDomElement( nodes.at( x ).toElement() );
        return elements;
    }
};

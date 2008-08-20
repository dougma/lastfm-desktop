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

#include "lib/DllExportMacro.h"
#include "UnicornException.h"
#include <QDebug>
#include <QDomElement>
#include <QList>
#include <QStringList>

/** @author <max@last.fm>
  * @brief facade pattern for QDomElement, throwing exceptions in situations that we must handle
  *
  * QDomElement dome;
  * EasyDomElement( dome )["album"]["image size=small"].text();
  * foreach (EasyDomElement e, EasyDomElement( dome )["album"].children( "image" ))
  *     qDebug() << e.text();
  */
class CORE_DLLEXPORT EasyDomElement
{
    QDomElement e;

	friend QDebug operator<<( QDebug, const EasyDomElement& );
	
public:
    class Exception : public UnicornException
    {
        friend class EasyDomElement;

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

    /** Selects a child element, you can specify attributes like so:
      *
      * e["element"]["element attribute=value"].text();
      */
    EasyDomElement operator[]( const QString& name ) const;
    
    /** use in all cases where empty would be an error, it throws if empty */
    QString nonEmptyText() const;

    QString text() const { return e.text(); }
    QList<EasyDomElement> children( const QString& named ) const;
};


inline QDebug operator<<( QDebug debug, const EasyDomElement& e )
{
	QString s;
	QTextStream t( &s, QIODevice::WriteOnly );
	e.e.save( t, 2 );
	return debug << s;
}

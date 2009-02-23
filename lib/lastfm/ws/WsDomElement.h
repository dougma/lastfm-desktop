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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#ifndef LASTFM_WS_DOM_ELEMENT_H
#define LASTFM_WS_DOM_ELEMENT_H

#include <lastfm/global.h>
#include <QDomElement>
#include <QList>
#include <QStringList>
#include <stdexcept>


/** @author <max@last.fm>
  * @brief facade pattern for QDomElement, throwing exceptions in situations that we must handle
  *
  * QDomElement dome;
  * WsDomElement( dome )["album"]["image size=small"].text();
  * foreach (WsDomElement e, WsDomElement( dome )["album"].children( "image" ))
  *     qDebug() << e.text();
  */
class LASTFM_WS_DLLEXPORT WsDomElement
{
    QDomElement e;

	friend QDebug operator<<( QDebug, const WsDomElement& );
    
    WsDomElement()
    {}
    
public:
    /** will throw std::runtime_error if the e.isNull() */
    WsDomElement( const QDomElement& e, const char* name = "" ) : e( e )
    {
        if (e.isNull()) throw std::runtime_error( "Expected node absent." + std::string(name) );
    }

    /** returns a null element unless the node @p name exists */
    WsDomElement optional( const QString& name ) const
    {
        try
        {
            return this->operator[]( name );
        }
        catch (std::runtime_error&)
        {
            return WsDomElement();
        }
    }
    
    /** Selects a child element, you can specify attributes like so:
      *
      * e["element"]["element attribute=value"].text();
      */
    WsDomElement operator[]( const QString& name ) const;
    
    /** use in all cases where empty would be an error, it throws if empty,
      * ignores any previous optional() usage, since you are explicitly asking
      * for a throw! */
    QString nonEmptyText() const;

    QString text() const { return e.text(); }
    QList<WsDomElement> children( const QString& named ) const;
    
    operator QDomElement() const { return e; }
};


inline QDebug operator<<( QDebug d, const WsDomElement& e )
{
	QString s;
	QTextStream t( &s, QIODevice::WriteOnly );
	e.e.save( t, 2 );
	return d << s;
}

#endif

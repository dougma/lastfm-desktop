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

#ifndef UNICORN_WIDGET_H
#define UNICORN_WIDGET_H
#include <QObject>
class UnicornWidget : public QObject
{
    //<undefined>
    UnicornWidget();
    ~UnicornWidget();
    UnicornWidget( const UnicornWidget& );
    //</undefined>
    
public:
    /** applies our custom palette, if you want item-views to have the black
      * look you must call this function on them explicitly, we don't recurse 
      * children in *that* case */
    static void paintItBlack( class QWidget* );
};


#include <QPointer>

#define UNICORN_UNIQUE_DIALOG_DECL( Type ) \
	QPointer<Type> m_the##Type;

#define PRIVATE_SETUP_UNICORN_UNIQUE_DIALOG( d ) \
	d->setAttribute( Qt::WA_DeleteOnClose ); \
	d->setWindowFlags( Qt::Dialog | Qt::WindowMinimizeButtonHint ); \
	d->setModal( false );

#define UNICORN_UNIQUE_DIALOG( Type ) \
	if (!m_the##Type) { \
		m_the##Type = new Type( this ); \
		PRIVATE_SETUP_UNICORN_UNIQUE_DIALOG( m_the##Type ); \
		m_the##Type->show(); \
	} else \
		m_the##Type->activateWindow();

#define UNICORN_UNIQUE_PER_TRACK_DIALOG( Type, t ) \
	if (!t.isNull() && (!m_the##Type || m_the##Type->track() != t)) { \
		m_the##Type = new Type( t, this ); \
		PRIVATE_SETUP_UNICORN_UNIQUE_DIALOG( m_the##Type ); \
		m_the##Type->show(); \
	} else \
		m_the##Type->activateWindow();

#endif

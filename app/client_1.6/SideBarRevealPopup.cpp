/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Christian Muehlhaeuser, Last.fm Ltd <chris@last.fm>                *
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

#include <QApplication>
#include <QMouseEvent>
#include "SideBarRevealPopup.h"


RevealPopup::RevealPopup( QWidget* parent ) :
        ToolTipLabel( parent )
{
    setMouseTracking( true );
}


bool
RevealPopup::event( QEvent *e )
{
    switch (e->type())
    {
        case QEvent::MouseMove:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        {
            // propogate mouse events up to the treeview as we are
            // a "transparent" widget, ie clicks should just pass thru
        
            #define e static_cast<QMouseEvent*>(e)
            QMouseEvent mouse_event( e->type(), 
                        parentWidget()->mapFromGlobal( e->globalPos() ),
                        e->globalPos(),
                        e->button(),
                        e->buttons(),
                        e->modifiers() );
            #undef e

            qApp->notify( parentWidget(), &mouse_event );
            return true;
        }
        
        case QEvent::Wheel:
        {
            // as above
            
            #define e static_cast<QWheelEvent*>(e)
            QWheelEvent wheel_event( e->pos(), 
                                     parentWidget()->mapFromGlobal( e->globalPos() ),
                                     e->delta(),
                                     e->buttons(),
                                     e->modifiers(),
                                     e->orientation() );
            #undef e
        
            qApp->notify( parentWidget(), &wheel_event );
            return true;
        }
        
        case QEvent::Paint:
            break;
        
        case QEvent::Hide:
            deleteLater();
            break;
        
        default:
            //qApp->notify( parentWidget(), e );
            break;
    }
    
    return ToolTipLabel::event( e );
}

/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Max Howell, Last.fm Ltd <max@last.fm>                              *
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

#include "SideBarToolTipLabel.h"
#include <QApplication>
#include <QKeyEvent>
#include <QStyle>
#include <QStylePainter>
#include <QStyleOptionFrame>
#include <QToolTip>


ToolTipLabel::ToolTipLabel( QWidget* parent ) :
        QLabel( parent, Qt::ToolTip )
{
    setMargin( 1 + style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, 0, this) );
    setFrameStyle(QFrame::NoFrame);
    setAlignment(Qt::AlignLeft);
    setIndent(1);
    setPalette( QToolTip::palette() );
}


void
ToolTipLabel::paintEvent( QPaintEvent *e )
{
    QStylePainter p( this );
    QStyleOptionFrame opt;
    opt.init( this );
    p.drawPrimitive( QStyle::PE_PanelTipLabel, opt );
    p.end();

    QLabel::paintEvent( e );
}


bool
ToolTipLabel::event( QEvent *e )
{
    switch (e->type()) {
        case QEvent::Show:
            qApp->installEventFilter( this );
            break;
            
        case QEvent::Hide:
            qApp->removeEventFilter( this );
            break;
    
        default:
            break;
    }
    
    return QLabel::event( e );
}

    
bool
ToolTipLabel::eventFilter( QObject *o, QEvent *e )
{
    switch (e->type()) {
        case QEvent::KeyPress:
        case QEvent::KeyRelease: 
        {
            int key = static_cast<QKeyEvent *>(e)->key();
            Qt::KeyboardModifiers mody = static_cast<QKeyEvent *>(e)->modifiers();
    
            if ((mody & Qt::KeyboardModifierMask) || (key == Qt::Key_Shift || key == Qt::Key_Control || key == Qt::Key_Alt || key == Qt::Key_Meta))
                break;
            
            hide();
            break;
        }

        case QEvent::Enter:
        case QEvent::Leave:
        {
            QPoint p = parentWidget()->mapFromGlobal( QCursor::pos() );
            
            if (parentWidget()->geometry().contains( p ))
                // we get these enter events, but we shouldn't delete as
                // then we'd dissappear when user is trying to read it!
                return true;
        } 
            //fall through
        
        case QEvent::DragLeave:
        case QEvent::DragEnter:
        case QEvent::WindowActivate:
        case QEvent::WindowDeactivate:
        case QEvent::FocusIn:
        case QEvent::FocusOut:
        case QEvent::Wheel:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
            hide();
            break;
    
        default:
            break;
    }

    return QLabel::eventFilter( o, e );
}

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

#include "UnicornWidget.h"
#include <QAbstractScrollArea>
#include <QApplication>
#include <QLabel>
#include <QPalette>
#include <QPushButton>
#include <QWidget>
#include <QAbstractItemView>


void //static
UnicornWidget::paintItBlack( QWidget* w )
{
    QPalette p = w->palette();
    p.setBrush( QPalette::Window, QColor( 35, 35, 35 ) );
    p.setBrush( QPalette::WindowText, QColor( 84, 84, 84 ) );
    
    if (qobject_cast<QAbstractItemView*>(w))
    {
        // reinterpret cast didn't work. Lol.
        p.setBrush( QPalette::Base, QColor( 35, 35, 35 ) );
        p.setBrush( QPalette::Text, Qt::white );
        p.setBrush( QPalette::Disabled, QPalette::Text, Qt::darkGray );
    }

    w->setPalette( p );
    
    foreach (QAbstractScrollArea* a, w->findChildren<QAbstractScrollArea*>())
        a->setFrameStyle( QFrame::NoFrame );

#ifdef Q_WS_MAC
    // Qt 4.4.1 on OS X is rubbish
    foreach (QLabel* l, w->findChildren<QLabel*>())
        l->setPalette( p );
    // unset palette for the aqua buttons, as otherwise when they are disabled
    // they are illegible
    foreach (QPushButton* b, w->findChildren<QPushButton*>())
    {
        // the following is documented to work
//        b->setPalette( QPalette() );
//        b->setAttribute( Qt::WA_SetPalette, false );
        // but only this works :(
        b->setPalette( qApp->palette() );
    }
#endif
}

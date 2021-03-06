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
#include "UnicornWidget.h"
#include <QAbstractScrollArea>
#include <QApplication>
#include <QLabel>
#include <QPalette>
#include <QPushButton>
#include <QWidget>
#include <QAbstractItemView>
#include <QTextEdit>

void //static
UnicornWidget::paintItBlack( QWidget* w )
{
    QPalette p = w->palette();
    p.setBrush( QPalette::Window, QColor( 0x0e0e0e ) );
    p.setBrush( QPalette::WindowText, QColor( 84, 84, 84 ) );
    
    if (qobject_cast<QAbstractItemView*>(w))
    {
        // reinterpret cast didn't work. Lol.
        p.setBrush( QPalette::Base, QColor( 0x0e0e0e ) );
        p.setBrush( QPalette::Text, Qt::white );
        p.setBrush( QPalette::Disabled, QPalette::Text, Qt::darkGray );
        p.setBrush( QPalette::AlternateBase, QColor( 0x0c0c0c ) );
    }
    
    if (qobject_cast<QTextEdit*>(w))
    {
        p.setBrush( QPalette::Base, QColor( 35, 35, 35 ) );
        p.setBrush( QPalette::Text, Qt::white );
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

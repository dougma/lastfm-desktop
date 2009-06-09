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
#include "QMessageBoxBuilder.h"
#include <QApplication>


QMessageBoxBuilder& 
QMessageBoxBuilder::setTitle( const QString& title )
{
#ifdef Q_WS_MAC
    box.setText( title + "\t\t\t" );
#else
    box.setWindowTitle( title );
#endif
    return *this;
}


QMessageBoxBuilder&
QMessageBoxBuilder::setText( const QString& text )
{
#ifdef Q_WS_MAC
    box.setInformativeText( text );
#else
    box.setText( text );
#endif
    return *this;
}


int
QMessageBoxBuilder::exec()
{
    QApplication::setOverrideCursor( Qt::ArrowCursor );
    int const r = box.exec();
    QApplication::restoreOverrideCursor();
    return r;
}

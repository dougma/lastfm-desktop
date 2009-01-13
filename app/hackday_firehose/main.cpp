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

#include "BigFirehoseDelegate.h"
#include "FirehoseView.h"
#include "FirehoseModel.h"
#include <QApplication>


int main( int argc, char** argv )
{
    QApplication app( argc, argv );
    
    FirehoseModel* model;
    
    FirehoseView view;
    
    QFont f = view.font();
    f.setPixelSize( 20 );
    view.setFont( f );
    
    view.setModel( model = new FirehoseModel );
    view.setDelegate( new BigFirehoseDelegate );
    
    
    
    model->setNozzle( "group/141180?rt=xml" );
    
    view.show();
    
    return app.exec();
}

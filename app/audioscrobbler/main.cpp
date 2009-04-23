/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *    This program is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include "_version.h"
#include "lib/unicorn/UniqueApplication.h"
#include "Application.h"


int main( int argc, char** argv )
{
    QCoreApplication::setApplicationName( "Audioscrobbler" );
    QCoreApplication::setApplicationVersion( VERSION );

#ifdef NDEBUG
    UniqueApplication uapp( moose::id() );
    if (uapp.isAlreadyRunning())
		return uapp.forward( argc, argv ) ? 0 : 1;
    uapp.init1();
#endif
	
    try
    {
        audioscrobbler::Application app( argc, argv );
      #ifdef NDEBUG
		uapp.init2( &app );
        app.connect( &uapp, SIGNAL(arguments( QStringList )), SLOT(parseArguments( QStringList )) );
      #endif
        return app.exec();
    }
    catch (unicorn::Application::StubbornUserException&)
    {
        // user wouldn't log in
        return 0;
    }
}

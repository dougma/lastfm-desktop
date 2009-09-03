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
#include "lib/unicorn/UnicornApplication.h"
#include <lastfm/ws.h>

namespace moralistfad
{
	class Application : public unicorn::Application
	{
	    Q_OBJECT
   
	public:
	    Application( int&, char** );

	signals:    
	    /** something should show it. Currently MainWindow does */
	    void error( const QString& message );
	    void status( const QString& message, const QString& id );

	public slots:
		void parseArguments( const QStringList& args );

	private slots:    
	    /** all webservices connect to this and emit in the case of bad errors that
	     * need to be handled at a higher level */
	    void onWsError( lastfm::ws::Error );
	};
}

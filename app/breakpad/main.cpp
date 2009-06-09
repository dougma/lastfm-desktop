/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole, Erik Jaelevik, 
        Christian Muehlhaeuser

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
#include "CrashReporter.h"
#include <iostream>

const char* kUsage = "Usage:\n"
                      "  CrashReporter <logDir> <dumpFileName> <productName> <username>\n";

int main( int argc, char** argv )
{
    Unicorn::Application app( argc, argv );
    Unicorn::Settings s;

    if (!s.isCrashReportingEnabled())
        return 0;

    if (app.arguments().size() != 4)
    {
        std::cout << kUsage;
        return 1;
    }

    CrashReporter reporter( app.arguments() );
    reporter.show();

    return app.exec();
}

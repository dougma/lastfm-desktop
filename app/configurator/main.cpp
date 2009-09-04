/*
   Copyright 2005-2009 Last.fm Ltd. 

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
#ifdef __APPLE__
    // first to prevent compilation errors with Qt 4.5.0
    //TODO shorten this mother fucker
    //NOTE including Carbon/Carbon.h breaks things as it has sooo many symbols
    //     in the global namespace
    #include </System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/AE.framework/Versions/A/Headers/AppleEvents.h>
    static pascal OSErr appleEventHandler( const AppleEvent*, AppleEvent*, long );
#endif

#include "_version.h"
#include <lastfm.h>
#include "lib/unicorn/UnicornApplication.h"
#include "lib/unicorn/UniqueApplication.h"
#include "lib/unicorn/QMessageBoxBuilder.h"
#include "SettingsDialog.h"


class MainObj : public QObject
{
    Q_OBJECT

public slots:
    void onArguments(const QStringList& args)
    {
        // well here we are.
        QMessageBoxBuilder box(0);
        box.setTitle("args received");
        box.setText(args.join(", "));
        box.exec();
    }
};



int main( int argc, char** argv )
{
    try
    {
        QCoreApplication::setApplicationName("Last.fm Configurator");
        QCoreApplication::setApplicationVersion(VERSION);

        UniqueApplication uapp("Configurator-68B34A74-D9F0-4309-A5A9-032423CB5DE2");
        if (uapp.isAlreadyRunning())
		    return uapp.forward(argc, argv) ? 0 : 1;

        uapp.init1();
	    unicorn::Application app(argc, argv);
		uapp.init2(&app);
        MainObj mainObj;
        mainObj.connect(&uapp, SIGNAL(arguments(QStringList)), SLOT(onArguments(QStringList)));
		
        SettingsDialog settingsDlg;
        settingsDlg.show();
        return app.exec();
    }
    catch (std::exception& e)
    {
        qDebug() << "unhandled exception " << e.what();
    }
}

#include "main.moc"

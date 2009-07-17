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
#ifdef __APPLE__
    // first to prevent compilation errors with Qt 4.5.0
    //TODO shorten this mother fucker
    //NOTE including Carbon/Carbon.h breaks things as it has sooo many symbols
    //     in the global namespace
    #include </System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/AE.framework/Versions/A/Headers/AppleEvents.h>
    static pascal OSErr appleEventHandler( const AppleEvent*, AppleEvent*, long );
#endif

#include "_version.h"
#include "Application.h"
#include "ScrobSocket.h"
#include "lib/unicorn/UniqueApplication.h"
#include "lib/unicorn/UnicornApplication.h"
#include "MainWindow.h"
#include "Radio.h"
#include "app/moose.h"

void setupRadio();
void cleanup();

class QMainObject : public QObject
{
	Q_OBJECT
	
public slots:
    void onStartRadio(RadioStation rs)
	{
		radio->play( rs );
	}
};

Radio* radio;
QMainObject* q;

namespace lastfm
{
    extern LASTFM_DLLEXPORT QByteArray UserAgent;
}

int main( int argc, char** argv )
{
    QCoreApplication::setApplicationName( "Last.fm Radio" );
    QCoreApplication::setApplicationVersion( VERSION );

    // ATTENTION! Under no circumstance change these strings! --mxcl
#ifdef WIN32
    lastfm::UserAgent = "Last.fm Client " VERSION " (Windows)";
#elif __APPLE__
    lastfm::UserAgent = "Last.fm Client " VERSION " (OS X)";
#elif defined (Q_WS_X11)
    lastfm::UserAgent = "Last.fm Client " VERSION " (X11)";
#endif

#ifdef NDEBUG
    UniqueApplication uapp( moose::id() );
    if (uapp.isAlreadyRunning())
		return uapp.forward( argc, argv ) ? 0 : 1;
    uapp.init1();
#endif
	
    try
    {
        moralistfad::Application app( argc, argv );
		q = new QMainObject;
      #ifdef NDEBUG
		uapp.init2( &app );
        q->connect( &uapp, SIGNAL(arguments( QStringList )), SLOT(parseArguments( QStringList )) );
      #endif
		
        setupRadio();
        qAddPostRoutine(cleanup);
        app.connect( radio, SIGNAL(error(int, QVariant)), SLOT(onRadioError(int, QVariant)) );

        ScrobSocket* scrobsock = new ScrobSocket("ass");
        scrobsock->connect(radio, SIGNAL(trackStarted(Track)), SLOT(start(Track)));
        scrobsock->connect(radio, SIGNAL(stopped()), SLOT(stop()));

      #ifdef Q_WS_MAC
        AEEventHandlerUPP h = NewAEEventHandlerUPP( appleEventHandler );
        AEInstallEventHandler( 'GURL', 'GURL', h, 0, false );
      #endif
        
        MainWindow window;

        q->connect(&window, SIGNAL(startRadio(RadioStation)), SLOT(onStartRadio(RadioStation)));

        window.setWindowTitle( app.applicationName() );

        window.show();

        app.parseArguments( app.arguments() );
        int result = app.exec();
        scrobsock->stop();
        return result;
    }
    catch (unicorn::Application::StubbornUserException&)
    {
        // user wouldn't log in
        return 0;
    }
}

#ifdef Q_WS_MAC
static pascal OSErr appleEventHandler( const AppleEvent* e, AppleEvent*, long )
{
    OSType id = typeWildCard;
    AEGetAttributePtr( e, keyEventIDAttr, typeType, 0, &id, sizeof(id), 0 );
    
    switch (id)
    {
        case 'GURL':
        {
            DescType type;
            Size size;

            char buf[1024];
            AEGetParamPtr( e, keyDirectObject, typeChar, &type, &buf, 1023, &size );
            buf[size] = '\0';

            QUrl const url = QString::fromUtf8( buf );
            radio->play( RadioStation(url) );
            return noErr;
        }
            
        default:
            return unimpErr;
    }
}
#endif


#include <phonon/audiooutput.h>
#include <phonon/backendcapabilities.h>

void setupRadio()
{
	Phonon::AudioOutput* audioOutput = new Phonon::AudioOutput( Phonon::MusicCategory, qApp );
//	audioOutput->setVolume( QSettings().value( "Volume", 80 ).toUInt() );

    qDebug() << audioOutput->name();
    qDebug() << audioOutput->outputDevice().description();
    qDebug() << audioOutput->outputDevice().name();
    qDebug() << (audioOutput->isMuted() ? "muted,": "not-muted,") <<
                (audioOutput->isValid() ? "valid,": "not-valid,") <<
                audioOutput->volumeDecibel() << "db " <<
                audioOutput->volume();
    foreach (QByteArray a, audioOutput->outputDevice().propertyNames()) {
        qDebug() << a << ":" << audioOutput->outputDevice().property(a);
    }

    QString audioOutputDeviceName = "";//TODO moose::Settings().audioOutputDeviceName();
    if (audioOutputDeviceName.size())
    {
        foreach (Phonon::AudioOutputDevice d, Phonon::BackendCapabilities::availableAudioOutputDevices())
            if (d.name() == audioOutputDeviceName) {
                audioOutput->setOutputDevice( d );
                break;
            }
    }
	radio = new Radio( audioOutput );
}



void cleanup()
{
	QSettings().value( "Volume", radio->audioOutput()->volume() );
}


#include "main.moc"

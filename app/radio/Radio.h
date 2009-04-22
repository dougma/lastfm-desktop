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

#include <lastfm/Track>
#include <lastfm/RadioStation>
#include <lastfm/WsError>
#include <phonon/phononnamespace.h>
#include <QList>
#include <QPointer>
#include <QThread>
#include <QVariant>
class Tuner;

namespace Phonon
{
	class MediaObject;
	class AudioOutput;
	class MediaSource;
}


/** @author <max@last.fm>
 */
class Radio : public QObject
{
    Q_OBJECT
    
public:
    Radio( Phonon::AudioOutput* );
    ~Radio();
	
	enum State
	{
        Stopped,
		TuningIn,
        Buffering,
		Playing
	};

	State state() const { return m_state; }

    Phonon::AudioOutput* audioOutput() const { return m_audioOutput; }

public slots:
    void play( const RadioStation& station );
    void skip();
    void stop();

signals:
    /** emitted up to twice, as first time may not have a title for the station
      * but the second time will */
    void tuningIn( const RadioStation& );
    void trackSpooled( const Track& );
    void trackStarted( const Track& );
    void buffering( int );
    void stopped();
	
	/** the error is either one of Ws::Error, or if Ws::Unknown a fatal error
      * from Phonon with error message set in data */
	void error( int, const QVariant& data = QVariant() );
    void tick( qint64 );

private slots:
    void enqueue();
    void onPhononStateChanged( Phonon::State, Phonon::State );
	void onPhononCurrentSourceChanged( const Phonon::MediaSource &);
	void onTunerError( Ws::Error );
    void phononEnqueue();
    void onBuffering( int );

	/** we get a "proper" station name from the tune webservice */
	void setStationNameIfCurrentlyBlank( const QString& );
	
private:
    /** resets internals to what Stopped means, used by changeState() */
    void clear();
    
	/** emits signals if appropriate */
	void changeState( State );
	
	QPointer<Tuner> m_tuner;
	Phonon::AudioOutput* m_audioOutput;
	Phonon::MediaObject* m_mediaObject;
	Radio::State m_state;
	Track m_track;
	RadioStation m_station;
    bool m_bErrorRecover;
};


#include <QDebug>
inline QDebug operator<<( QDebug d, Radio::State s )
{
    return d << lastfm::qMetaEnumString<Radio>( s, "State" );
}
inline QDebug operator<<( QDebug d, Phonon::State s )
{
	switch (s)
	{
	    #define CASE(x) case x: return d << #x
		CASE(Phonon::LoadingState);
		CASE(Phonon::StoppedState);
		CASE(Phonon::PlayingState);
		CASE(Phonon::BufferingState);
		CASE(Phonon::PausedState);
		CASE(Phonon::ErrorState);
        #undef CASE
	}
    return d;
}


Q_DECLARE_METATYPE( Radio::State );


extern Radio* radio;

#ifndef RADIOCONTROLLER_H
#define RADIOCONTROLLER_H

#include <QList>
#include <QObject>
#include "lib/types/Track.h"
#include "lib/DllExportMacro.h"
class AudioPlaybackEngine;
class RadioStation;
class Tuner;
class Track;


class RADIO_DLLEXPORT RadioController : public QObject
{
    Q_OBJECT

public:
    RadioController();
	//TODO remove, instead return a volume slider that is configured and ready
	AudioPlaybackEngine* audioPlaybackEngine() const { return m_audio; }
    
public slots:
    void play( const RadioStation& );
    void stop();
    void skip();

private slots:
    void enqueue( const QList<Track>& );

signals:
	void tuningIn( const QString& title );
	void tuned( const QString& title );
	void preparing( const Track& );
	/** buffering and percentage, you can get this mid-track too, if so, see playbackResumed() */
	void buffering( int );
    void trackStarted( const Track& );
	/** stop() was called or a serious radio error occurred */
    void playbackEnded();
	
    /** if the buffers empty mid track, you'll get buffering */
    void playbackResumed();

private:
	Tuner* m_tuner;
    AudioPlaybackEngine* m_audio;
	QString m_currentStation;
};

#endif //RADIOCONTROLLER_H

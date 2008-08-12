#ifndef RADIOCONTROLLER_H
#define RADIOCONTROLLER_H

#include <QObject>
#include "RadioStation.h"
#include "lib/DllExportMacro.h"
class AudioPlaybackEngine;


class RADIO_DLLEXPORT RadioController : public QObject
{
    Q_OBJECT

public:
    RadioController();
	AudioPlaybackEngine* audioPlaybackEngine() const { return m_audio; }
    
public slots:
    void play( const RadioStation& s = RadioStation( "", RadioStation::SimilarArtist ) );
    void stop();
    void skip();

private slots:
    void onQueueStarved();

signals:
    void trackStarted( const class Track& );
    /** queue more tracks or... */
    void queueStarved();
    /** ...playback ends */
    void playbackEnded();
    /** buffering has occured mid-track */
    void buffering();
    /** when the player has finished buffering mid-track */
    void finishedBuffering();

	void tuningStateChanged( bool );
	
	void newStationTuned( const QString& );

private:
    AudioPlaybackEngine* m_audio;
	QString m_currentStation;
};

#endif //RADIOCONTROLLER_H

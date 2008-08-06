#ifndef RADIOCONTROLLER_H
#define RADIOCONTROLLER_H

#include <QObject>
#include "RadioStation.h"
#include "lib/DllExportMacro.h"
#include "lib/types/Track.h"

class RADIO_DLLEXPORT RadioController: public QObject
{
    Q_OBJECT
    
    public:
    RadioController();
	class AudioPlaybackEngine* audioPlaybackEngine(){ return m_audio; }
    
    public slots:
    void play( const class RadioStation& s = RadioStation( "" ) );
    void stop();
    void skip();
    
    private slots:
    void onQueueStarved();
    
    signals:
    void trackStarted( const Track& );
    /** queue more tracks or... */
    void queueStarved();
    /** ...playback ends */
    void playbackEnded();
    /** buffering has occured mid-track */
    void buffering();
    /** when the player has finished buffering mid-track */
    void finishedBuffering();
	
	void tuningStateChanged( bool );
    
    private:
    class AudioPlaybackEngine* m_audio;
};

#endif //RADIOCONTROLLER_H

#ifndef SCROBBLE_STATUS_H_
#define SCROBBLE_STATUS_H_

#include "lib/unicorn/StylableWidget.h"
#include <lastfm/Track>
#include <QPointer>

class StopWatch;
class ScrobbleStatus : public StylableWidget 
{
Q_OBJECT
public:
    ScrobbleStatus( QWidget* parent = 0 );

protected:
    virtual void paintEvent ( class QPaintEvent* );

private:
    struct {
        class QLabel* as;
        class QLabel* title;
        class QLabel* playerStatus;
    } ui;

    QPointer<StopWatch> m_stopWatch;
    QTimer* m_timer;

public slots:
    void onTrackStarted( const Track&, const Track& );

private slots:
    void onWatchPaused( bool );
    void onWatchFinished();
};

#endif //SCROBBLE_STATUS_H_

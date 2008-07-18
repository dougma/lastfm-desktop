#ifndef NOWPLAYINGVIEW_H
#define NOWPLAYINGVIEW_H

#include <QWidget>
#include "ObservedTrack.h"
#include "ui_NowPlayingView.h"

class NowPlayingView : public QWidget
{
    Q_OBJECT

public:
    NowPlayingView(QWidget *parent = 0);
    ~NowPlayingView();

private slots:
    void onAppEvent( int, const QVariant& );

private:
    Ui::NowPlayingView ui;
    void setCurrentTrack( const ObservedTrack& track );
};

#endif // NOWPLAYINGVIEW_H

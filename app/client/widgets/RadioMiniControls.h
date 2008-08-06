#ifndef RADIOMINICONTROLS_H
#define RADIOMINICONTROLS_H

#include <QWidget>
#include "ui_RadioMiniControls.h"

namespace Phonon{ class VolumeSlider; }

class RadioMiniControls : public QWidget
{
    Q_OBJECT

public:
    RadioMiniControls(QWidget *parent = 0);
    ~RadioMiniControls();

    void setAudioPlaybackEngine( class AudioPlaybackEngine* o );
    
public slots:
    void play();
	void onRadioToggled();

signals:
	void radioToggled();

private:
    struct : Ui::RadioMiniControls
    {
        Phonon::VolumeSlider* volumeSlider;
    } ui;
};

#endif // RADIOMINICONTROLS_H

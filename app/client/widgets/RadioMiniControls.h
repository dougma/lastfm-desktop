#ifndef RADIOMINICONTROLS_H
#define RADIOMINICONTROLS_H

#include <QWidget>
#include "ui_RadioMiniControls.h"

namespace Phonon{ class VolumeSlider; class AudioOutput; }

class RadioMiniControls : public QWidget
{
    Q_OBJECT

public:
    RadioMiniControls(QWidget *parent = 0);
    ~RadioMiniControls();

    void setAudioOutput( Phonon::AudioOutput* o );

private:
    struct : Ui::RadioMiniControls
    {
        Phonon::VolumeSlider* volumeSlider;
    } ui;
};

#endif // RADIOMINICONTROLS_H

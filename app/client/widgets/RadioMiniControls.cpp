#include "RadioMiniControls.h"
#include <phonon/volumeslider.h>
#include <phonon/audiooutput.h>

RadioMiniControls::RadioMiniControls(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    ui.volumeSlider = new Phonon::VolumeSlider( new Phonon::AudioOutput(Phonon::MusicCategory, this), this );
    layout()->addWidget( ui.volumeSlider );
}

RadioMiniControls::~RadioMiniControls()
{

}

void
RadioMiniControls::setAudioOutput( Phonon::AudioOutput* o )
{
    ui.volumeSlider->setAudioOutput( o );
}
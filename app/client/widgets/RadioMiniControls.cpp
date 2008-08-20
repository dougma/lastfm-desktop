#include "RadioMiniControls.h"
#include "lib/radio/AudioPlaybackEngine.h"
#include <phonon/volumeslider.h>

RadioMiniControls::RadioMiniControls(QWidget *parent)
    : QFrame(parent)
{
    ui.setupUi(this);

    ui.volumeSlider = new Phonon::VolumeSlider( this );
	ui.radio->setAutoExclusive( true );
	ui.scrobble->setAutoExclusive( true );
	
	
    layout()->addWidget( ui.volumeSlider );
	connect( ui.scrobble, SIGNAL( clicked()), SIGNAL( radioToggled()) );
	connect( ui.radio, SIGNAL( clicked()), SIGNAL( radioToggled()) );
}


RadioMiniControls::~RadioMiniControls()
{

}


void
RadioMiniControls::setAudioPlaybackEngine( AudioPlaybackEngine* a )
{
    ui.volumeSlider->setAudioOutput( a->audioOutput() );
    connect( ui.play, SIGNAL( clicked() ), SLOT( play() ));
    connect( ui.skip, SIGNAL( clicked() ), a, SLOT( skip() ) );
}


void
RadioMiniControls::play()
{
}


void
RadioMiniControls::onRadioToggled()
{
	ui.scrobble->toggle();
}
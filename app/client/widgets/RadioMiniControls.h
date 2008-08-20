#ifndef RADIOMINICONTROLS_H
#define RADIOMINICONTROLS_H

#include <QWidget>


#include <QPushButton>
#include <QPainter>
#include <QPaintEvent>
class ImageButton : public QPushButton
{
	Q_OBJECT
public:
	ImageButton( QWidget* parent ) : QPushButton( parent ){};
	
	void paintEvent ( QPaintEvent* event )
	{
		QPainter p(this);
		
		if( isDown() )
			p.setCompositionMode( QPainter::CompositionMode_Exclusion );
		
		p.drawPixmap( event->rect(), icon().pixmap( event->rect().width(), event->rect().height()) );
	}
};

#include "ui_RadioMiniControls.h"

namespace Phonon{ class VolumeSlider; }

class RadioMiniControls : public QFrame
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

/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *    This program is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include "RadioWidget.h"
#include "lib/radio/RadioController.h"
#include "lib/unicorn/widgets/SpinnerLabel.h"
#include "MyStations.h"
#include <QAction>
#include <QEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMovie>
#include <QToolBar>
#include <QVBoxLayout>


RadioWidget::RadioWidget( QWidget* parent )
           : QWidget( parent )
{
    qRegisterMetaType<Track>( "Track" );

    QToolBar* bar = new QToolBar( this );

    QLineEdit* tuning_dial = new QLineEdit;
    skip = bar->addAction( "Skip" );
    stop = bar->addAction( "Stop" );

    QVBoxLayout* v = new QVBoxLayout( this );
    v->addWidget( bar );
    v->addWidget( tuning_dial );
	v->addWidget( new MyStations );
    v->addWidget( ui.spinner = new SpinnerLabel );

    connect( tuning_dial, SIGNAL(returnPressed()), SLOT(onTunerReturnPressed()) );
	
    ui.spinner->hide();

    setWindowTitle( tr("Last.fm Radio") );
}


void
RadioWidget::onTunerReturnPressed()
{
    QString url = static_cast<QLineEdit*>(sender())->text();
    m_radioController->play( RadioStation( url, RadioStation::SimilarArtist ) );
	emit newStationStarted();
}


void
RadioWidget::setRadioController( RadioController* r )
{
	m_radioController = r;
	connect( skip, SIGNAL(triggered()), m_radioController, SLOT(skip()) );
	connect( stop, SIGNAL(triggered()), m_radioController, SLOT(stop()) );
	connect( m_radioController, SIGNAL(tuningStateChanged(bool)), ui.spinner, SLOT(setVisible(bool)) );
}
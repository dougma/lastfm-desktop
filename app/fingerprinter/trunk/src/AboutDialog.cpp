/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Christian Muehlhaeuser, Last.fm Ltd <chris@last.fm>                *
 *      Erik Jaelevik, Last.fm Ltd <erik@last.fm>                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02111-1307, USA.          *
 ***************************************************************************/

#include "AboutDialog.h"
#include "containerutils.h"
#include "logger.h"
#include "FingerprinterSettings.h"

#include <QtGui>

AboutDialog::AboutDialog( QWidget* parent )
        : QDialog( parent )
{
    ui.setupUi( this );
    ui.line->setFrameShadow( QFrame::Sunken ); // Want etched, not flat

#ifdef WIN32
    bool bOK = m_watermark.load( dataPath( "about.png" ) );
#endif
#ifdef Q_WS_MAC
    bool bOK = m_watermark.load( dataPath( "about_mac.png" ) );
#endif
#ifdef Q_WS_X11
    bool bOK = m_watermark.load( dataPath( "about_generic.png" ) );
#endif

    if (!bOK)
        LOG( 2, "Could not load About watermark\n" );

    QString labelText = tr( "Version %1" ).arg( FingerprinterSettings::instance().version() );

    labelText += '\n' + tr( "Copyright 2007 Last.fm Ltd. (C)" );
    ui.labelInfo->setText( labelText );

    adjustSize();
    setFixedSize( sizeHint() );
}


void
AboutDialog::paintEvent( QPaintEvent* /*event*/ )
{
    QPainter painter( this );
    painter.drawPixmap( 0, 0, m_watermark );
    painter.end();
}

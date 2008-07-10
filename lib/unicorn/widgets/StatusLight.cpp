/***************************************************************************
 *   Copyright (C) 2008 by P. Sereno                                       *
 *   http://www.sereno-online.com                                          *
 *   http://www.qt4lab.org                                                 *
 *   http://www.qphoton.org                                                *
 *   Last.fm Ltd. <max@last.fm>                                            *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
  
#include "StatusLight.h"
#include <QDebug>
#include <QFontMetrics>
#include <QPainter>
#include <QRadialGradient>


StatusLight::StatusLight( QWidget* parent )
           : QWidget( parent )
{
    m_color = Qt::white;

    int const M = fontMetrics().height();
    setFixedSize( M, M );

    qDebug() << M;
}


void StatusLight::paintEvent( QPaintEvent* )
{
	QPainter p( this );
	p.setRenderHint( QPainter::Antialiasing, true );
	p.setWindow( -50,-50, 100, 100 );
	
	p.setPen(Qt::white);
	p.drawArc(-25,-25,50,50,0,5670);
	p.drawArc(-32,-33,66,66,0,5670);
	p.setPen(Qt::darkGray);
	p.drawArc(-34,-33,66,66,3400,3000);
	
    QRadialGradient radialGrad(QPointF(-8, -8), 20);
    radialGrad.setColorAt(0, Qt::white);
    radialGrad.setColorAt(1, m_color);

    QBrush brush(radialGrad);
    p.setBrush(brush);
    p.setPen(Qt::black);
    p.drawEllipse(-25,-25,50,50);
}

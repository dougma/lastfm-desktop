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
#include <QColor>
#include <QtGlobal>
#include <QtGui>
#include <QPolygon>


StatusLight::StatusLight( QWidget *parent )
    : QWidget( parent )
{
   m_value=false;
   m_color=Qt::red;
   setMinimumSize(QSize(50,50));
}


void StatusLight::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setWindow( -50,-50,100,100);
	
	painter.setPen(Qt::white);
	painter.drawArc(-25,-25,50,50,0,5670);
	painter.drawArc(-32,-33,66,66,0,5670);
	painter.setPen(Qt::darkGray);
	painter.drawArc(-34,-33,66,66,3400,3000);
	
    if(m_value)
    {
      QRadialGradient radialGrad(QPointF(-8, -8), 20);
      radialGrad.setColorAt(0, Qt::white);
       
      radialGrad.setColorAt(1, m_color);
   	  QBrush brush(radialGrad);
      painter.setBrush(brush);
      painter.setPen(Qt::black);
	  painter.drawEllipse(-25,-25,50,50);
    }
    else
    {
      QRadialGradient radialGrad(QPointF(-8, -8), 20);
      radialGrad.setColorAt(0, Qt::white);
      radialGrad.setColorAt(1, Qt::lightGray);
   	  QBrush brush(radialGrad);
      painter.setBrush(brush);
	  painter.drawEllipse(-25,-25,50,50);
    }	
}

void StatusLight::setColor(QColor newColor)
{
   m_color=newColor;
   update();
}


void StatusLight::setValue(bool value)
{
   m_value=value;
   update();
}


void StatusLight::toggleValue()
{ 
	m_value=!m_value;
	update();
	return; 
}

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

// note this was QLed, see above links for original source package

#ifndef STATUS_LIGHT_H
#define STATUS_LIGHT_H

#include <QWidget>

class StatusLight : public QWidget
{
    Q_OBJECT

public: 
    StatusLight( QWidget *parent = 0 );
    bool value() const { return m_value; }
    QColor color() const { return m_color; }
    
public slots:
	void setValue(bool);
	void setColor(QColor);
	void toggleValue();

protected:
    bool m_value;
    QColor m_color;
    void paintEvent(QPaintEvent *event);

};

#endif

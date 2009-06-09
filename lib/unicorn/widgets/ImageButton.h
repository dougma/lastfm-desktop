/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef IMAGE_BUTTON_H
#define IMAGE_BUTTON_H

#include "ActionButton.h"
#include <QIcon>
#include <QMap>


class ImageButton : public ActionButton
{
    void init( const QPixmap& );
	
public:
    /** this pixmap becomes the rest state pixmap and defines the size of the eventual widget */
    explicit ImageButton( const QPixmap& pixmap, QWidget* parent = 0 );
    explicit ImageButton( const QString& pixmap_path, QWidget* parent = 0 );
    
    void setPixmap( const QPixmap&, const QIcon::State = QIcon::Off, QIcon::Mode = QIcon::Normal );
	void moveIcon( int x, int y, QIcon::Mode = QIcon::Normal );
    
	virtual QSize sizeHint() const { return m_sizeHint; }    
    
protected:
	virtual void paintEvent( QPaintEvent* event );
    
private:
    QSize m_sizeHint;
	QMap< QIcon::Mode, QPoint > m_iconOffsets;
};

#endif //IMAGE_BUTTON_H
/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#ifndef IMAGE_BUTTON_H
#define IMAGE_BUTTON_H

#include <QAbstractButton>
#include <QAction>
#include <QIcon>
#include <QMap>


class ImageButton : public QAbstractButton
{
	Q_OBJECT
	
public:
    ImageButton( QWidget* parent = 0 );
	ImageButton( const QString& path );

	void setAction( class QAction* );
    
	virtual void paintEvent ( QPaintEvent* event );
	
	virtual QSize sizeHint() const;
	void setPixmap( const QString&, const QIcon::State = QIcon::Off );
	void setBackgroundPixmap( const QString&, const QIcon::Mode = QIcon::Normal );
	void moveIcon( int x, int y, QIcon::Mode = QIcon::Normal );
    
private:
	QIcon m_backgroundIcon;
	QMap< QIcon::Mode, QPoint > m_iconOffsets;
	
    void setIconSize( const QSize& s )
    {
        setProperty( "iconSize", s );
    }    
    
private slots:
	void actionChanged();
};

#endif //IMAGE_BUTTON_H
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
#include <QPushButton>
#include <QPainter>
#include <QPaintEvent>

class ImageButton : public QPushButton
{
	Q_OBJECT
	
public:
	ImageButton( QWidget* parent ) : QPushButton( parent )
	{}
	
	void paintEvent ( QPaintEvent* event )
	{
		QPainter p( this );
		
		if( isDown() )
			p.setCompositionMode( QPainter::CompositionMode_Exclusion );
		
		QIcon::Mode state = isEnabled() ? QIcon::Normal : QIcon::Disabled;
		
		p.setClipRect( event->rect() );
		
		QIcon i;
		if( isChecked() && !m_checkedIcon.isNull() )
			i = m_checkedIcon;
		else
			i = icon();
		
		i.paint( &p, rect(), Qt::AlignTop, state );
	}
	
	void setCheckedIcon( const QIcon& i ){ m_checkedIcon = i; }
	
private:
	QIcon m_checkedIcon;
};
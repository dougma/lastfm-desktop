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

#include <QPushButton>
#include <QPainter>
#include <QPaintEvent>
#include <QLayout>
#include <QAction>
#include <QPixmap>
#include <QIcon>
#include <QString>
#include <QDebug>

class ImageButton : public QPushButton
{
	Q_OBJECT
	
public:
	ImageButton( QWidget* parent ) : QPushButton( parent ), iconX( 0 ), iconY( 0 )
	{
			setIconSize( QSize(150, 150) );
	}
	
	ImageButton( const QString& path, QAction* action = 0 ) : iconX( 0 ), iconY( 0 )
	{
		if( action )
		{
			connect( this, SIGNAL(clicked()), action, SLOT( trigger()) );
			connect( action, SIGNAL(changed()), SLOT( actionChanged()) );
			setEnabled( action->isEnabled() );
			setChecked( action->isChecked() );
		}
		
        QPixmap disabled( path.left( path.length() - 4 ) + "_inactive.png" );
        
        if (!disabled.isNull())
        {
            QPixmap p( path );
            QIcon i( p );
            i.addPixmap( disabled, QIcon::Disabled );
			setIcon( i );
        }
        
		setIconSize( QSize(150, 150) );
	}
	
	virtual void paintEvent ( QPaintEvent* event )
	{
		QPainter p( this );
		
		QIcon::Mode mode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
		if( isDown() )
			mode = QIcon::Active;
		
		QIcon::State state = isChecked() ? QIcon::On : QIcon::Off;
		
		p.setClipRect( event->rect() );
		
		m_backgroundIcon.paint( &p, rect(), Qt::AlignCenter, mode, state );

		QRect iconRect = rect();
		iconRect.setLeft( iconRect.left() + iconX );
		iconRect.setBottom( iconRect.bottom() + iconY - 3 );
		icon().paint( &p, iconRect, Qt::AlignCenter, mode, state );

	}
	
	virtual QSize sizeHint() const
	{
		return icon().actualSize( iconSize()).expandedTo( m_backgroundIcon.actualSize(iconSize()));
	}
	
	void setPixmap( const QString& s, const QIcon::State st = QIcon::Off ){ setPixmap( QPixmap( s ), st ); }
	void setPixmap( const QPixmap& p, const QIcon::State s = QIcon::Off )
	{
		QIcon i = icon();
		i.addPixmap( p, QIcon::Normal, s );
		setIcon( i );
	}
	
	void setBackgroundPixmap( const QString& s, const QIcon::Mode m = QIcon::Normal ){ setBackgroundPixmap( QPixmap( s ), m ); }
	void setBackgroundPixmap( const QPixmap& p, const QIcon::Mode m = QIcon::Normal )
	{
		m_backgroundIcon.addPixmap( p, m );
	}
	
	void moveIcon( int x, int y ){ iconX += x; iconY += y; }
	
private:
	QIcon m_backgroundIcon;
	int iconX, iconY;
	
private slots:
	virtual void actionChanged()
	{
		QAction* action = static_cast<QAction*> (sender());
		setEnabled( action->isEnabled());
		setChecked( action->isChecked());
	}
};

#endif //IMAGE_BUTTON_H
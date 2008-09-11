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
 
#ifndef SCROBBLE_VIEW_WIDGET_H
#define SCROBBLE_VIEW_WIDGET_H

#include <QWidget>
#include <QSplitter>
#include <QPainter>
#include <QPaintEvent>

namespace Ui
{
	class MainWindow;
}


class ScrobbleViewWidget : public QWidget
{
public:
	ScrobbleViewWidget( Ui::MainWindow&, QWidget* parent = 0 );

	struct {
		class ImageButton* love;
		class ImageButton* ban;
		class ImageButton* tag;
		class ImageButton* share;
		class ImageButton* cog;
	} ui;
};


class PaintedSplitter : public QSplitter
{
public:
	PaintedSplitter( Qt::Orientation o ): QSplitter( o ){};
	
protected:
	QSplitterHandle* createHandle(){ return new PaintedSplitterHandle( orientation(), this ); }
	
private:
	class PaintedSplitterHandle : public QSplitterHandle
	{
	public:
		PaintedSplitterHandle( Qt::Orientation o, QSplitter* p ): QSplitterHandle( o, p ){};
		
		void paintEvent(QPaintEvent *event)
		{
			QLinearGradient gradient;
			QPainter painter(this);
			gradient.setColorAt( 0.0f, QColor( 47, 47, 47 ));
			gradient.setColorAt( 0.5f, QColor( 16, 16, 16 ) );
			gradient.setColorAt( 1.0f, Qt::black );
			
			gradient.setStart(rect().width()/2, rect().top());
			gradient.setFinalStop(rect().width()/2, rect().bottom());
			
			
			painter.setPen( Qt::black );
			painter.drawLine( rect().translated( 0, 2 ).topLeft(), rect().translated( 0, 2 ).topRight());
			painter.fillRect(event->rect(), QBrush(gradient));
			
			painter.drawImage( (rect().width() / 2) - 7, -1, QImage( ":/splitter_handle.png" ));
		}
	};
};

#endif //SCROBBLE_VIEW_WIDGET_H
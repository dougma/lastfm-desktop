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
#ifndef TRACK_INFO_WIDGET_H
#define TRACK_INFO_WIDGET_H

#include <QImage>
#include <QWidget>
#include <lastfm/Track>


class PrettyCoverWidget : public QWidget
{
    Q_OBJECT
    
public:
    PrettyCoverWidget();

	void clear();
    
    void setReflectionHeight( uint h ) { m_reflection_height = h; }
    uint reflectionHeight() const { return m_reflection_height; }
    
    int widthForHeight( int h ) const
    {
        return h - m_reflection_height;
    }

    int heightForWidth( int w ) const
    {
        return w + m_reflection_height;
    }
    
    void setMinimumWidth( int w )
    {
        setMinimumSize( w, heightForWidth( w ) );
    }

public slots:
    void setImage( const QImage& );
    
signals:
    void clicked();
    
protected:
    virtual void mouseReleaseEvent( QMouseEvent* );
    
private:
    void paintEvent( QPaintEvent* );

    QImage m_cover;
    uint m_reflection_height;
};

#endif //TRACK_INFO_WIDGET_H

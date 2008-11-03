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

#ifndef MULTI_BUTTON_POPUP_H
#define MULTI_BUTTON_POPUP_H

#include <QWidget>


class CogButtonPopup : public QWidget
{
    Q_OBJECT
    
    class QTimeLine* m_timeline;
    
public:
    CogButtonPopup( const int width, QWidget* parent );   
    
public slots:
    void move( int i )
    {
        QWidget::move( x(), parentWidget()->height() - i );
    }

    void praise();
    void curse();
    void bye();

signals:
    void addToPlaylistClicked();
    
private:
    virtual void paintEvent( QPaintEvent* );
};

#endif

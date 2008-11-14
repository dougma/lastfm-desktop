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
 
#ifndef UNICORN_SIZE_GRIP
#define UNICORN_SIZE_GRIP

#include <QSizeGrip>
#include <QPainter>
#include <QPaintEvent>
#include <QDebug>

class UnicornSizeGrip : public QSizeGrip
{
public:
    UnicornSizeGrip( QWidget* w, Qt::Alignment o = Qt::AlignRight | Qt::AlignBottom );
    
protected:
    Qt::Alignment m_orientation;
    QTransform m_transformMatrix;

    void paintEvent( QPaintEvent* );
    bool eventFilter( QObject*, QEvent* );
    
    int m_xAdjust1, m_xAdjust2, m_yAdjust1, m_yAdjust2;
    
//    void mouseMoveEvent( QMouseEvent* e )
//    {
//        if( !(e->buttons() & Qt::LeftButton) )
//            return QSizeGrip::mouseMoveEvent( e );
//        
//        foreach( QObject* o, parentWidget()->parentWidget()->children() )
//        {
//            QWidget* w;
//            if( o == parentWidget() || !(w = qobject_cast< QWidget* >( o )) )
//                continue;
//            
//            w->setFixedHeight( w->height() );
//        }
//        QSizeGrip::mouseMoveEvent( e );
//    }
};

#endif //UNICORN_SIZE_GRIP
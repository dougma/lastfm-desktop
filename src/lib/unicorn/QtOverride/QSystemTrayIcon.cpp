/***************************************************************************
 *   Copyright 2008 Last.fm Ltd.                                           *
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

#include "./QSystemTrayIcon"
#undef QSystemTrayIcon

#include "UnicornCommonMac.h"
#include "libUnicorn/AppleScript.h"
#include "libUnicorn/logger.h"
#include "MooseCommon.h"

#include <QBoxLayout>
#include <QDesktopWidget>
#include <QLabel>
#include <QPointer>
#include <QTimer>


class GrowlClone : public QWidget
{
public:
    GrowlClone( const QString& title, const QString& description );
    
protected:
    virtual void mousePressEvent( QMouseEvent* )
    {
        deleteLater();
    }
};


void
Moose::QSystemTrayIcon::showMessage( const QString& title,
                                     const QString& description,
                                     MessageIcon icon,
                                     int ms )
{
    if (!UnicornUtils::isGrowlInstalled())
    {
        static QPointer<GrowlClone> w;
        delete w;
        w = new GrowlClone( title, description );
    }
    else
        ::QSystemTrayIcon::showMessage( title, description, icon, ms );
}


GrowlClone::GrowlClone( const QString& title, const QString& description )
{
    QLabel* ltitle = new QLabel( title );
    QLabel* ldescription = new QLabel( description );
    QLabel* logo = new QLabel;

    logo->setPixmap( Moose::dataPath( "app_55.png" ) );
    logo->setFixedSize( 55, 55 );

    ltitle->setFixedWidth( 200 );
    ldescription->setFixedWidth( 200 );
    ldescription->setWordWrap( true );

    setPalette( Qt::black ); //should be recursive
    ltitle->setPalette( palette() ); //but Qt docs
    ldescription->setPalette( palette() ); //lie
    
    QFont f = ltitle->font();
    f.setBold( true );
    ltitle->setFont( f );
    f.setBold( false );
    f.setPixelSize( 10 );
    ldescription->setFont( f );
    
    QVBoxLayout* v = new QVBoxLayout;
    v->addWidget( ltitle );
    v->addWidget( ldescription );
    v->setSpacing( 3 );
    
    QHBoxLayout* h = new QHBoxLayout( this );
    h->addWidget( logo );
    h->addLayout( v );
    h->setMargin( 12 );
    h->setSizeConstraint( QLayout::SetFixedSize );
    h->setAlignment( logo, Qt::AlignCenter );
    h->setAlignment( v, Qt::AlignTop );
    
    setWindowFlags( Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint );
    setWindowOpacity( 0.85 );
    
    int x = QDesktopWidget().availableGeometry().right() - sizeHint().width() - 20;
    int y = 50;
    move( x, y );
    
    show();
    
    QTimer::singleShot( 10 * 1000, this, SLOT(deleteLater()) );
}

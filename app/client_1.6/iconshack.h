/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Christian Muehlhaeuser, Last.fm Ltd <chris@last.fm>                *
 *      Erik Jaelevik, Last.fm Ltd <erik@last.fm>                          *
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

#ifndef ICONSHACK_H
#define ICONSHACK_H

#include <QPixmap>
#include <QString>


/*************************************************************************/ /**
    Loads and dishes out icons.
******************************************************************************/
class IconShack
{
    IconShack() : mbIconsLoaded( false ) {}
    
public:
    ~IconShack()
    {
        Q_ASSERT( true );    
    }

    /*********************************************************************/ /**
        Singleton instance
    **************************************************************************/
    static IconShack& instance()
    {
        static IconShack instance;
        return instance;
    }

    /*********************************************************************/ /**
        Returns the standard user icon in the requested colour.
    **************************************************************************/
    QPixmap GetGoodUserIcon( MooseEnums::UserIconColour eColour );

    /*********************************************************************/ /**
        Returns the 32px user icon in the requested colour.
    **************************************************************************/
    QPixmap GetGoodUserIconCollapsed( MooseEnums::UserIconColour eColour );

    /*********************************************************************/ /**
        Returns the 32px user icon in the requested colour.
    **************************************************************************/
    QPixmap GetGoodUserIconExpanded( MooseEnums::UserIconColour eColour );

    /*********************************************************************/ /**
        Returns the exclamation mark user icon in the requested colour.
    **************************************************************************/
    QPixmap GetExclUserIcon( MooseEnums::UserIconColour eColour);

    /*********************************************************************/ /**
        Returns the cross user icon in the requested colour.
    **************************************************************************/
    QPixmap GetDisabledUserIcon( MooseEnums::UserIconColour eColour);

private:

    /*********************************************************************/ /**
        Load icons from resources.
    **************************************************************************/
    void LoadIcons();

    QPixmap icon( QPixmap[], MooseEnums::UserIconColour );

    QPixmap maGoodIcons[ MooseEnums::eColorMax ];
    QPixmap maGoodIconsCollapsed[ MooseEnums::eColorMax ];
    QPixmap maGoodIconsExpanded[ MooseEnums::eColorMax ];
    QPixmap maExclIcons[ MooseEnums::eColorMax ];
    QPixmap maDisabledIcons[ MooseEnums::eColorMax ];
    QList<QPixmap> mIpodScrobblingIcons[ MooseEnums::eColorMax ];

    bool mbIconsLoaded;
};

#endif // ICONSHACK_H

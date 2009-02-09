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

#include "iconshack.h"
#include <QPainter>


QPixmap
IconShack::icon( QPixmap icons[], MooseEnums::UserIconColour c )
{
    if (!mbIconsLoaded)
        LoadIcons();

    if (c == MooseEnums::eNone || icons[c].isNull())
        return icons[ MooseEnums::eRed ];
    else
        return icons[c];
}


QPixmap
IconShack::GetGoodUserIcon( MooseEnums::UserIconColour eColour )
{
    return icon( maGoodIcons, eColour );
}


QPixmap
IconShack::GetGoodUserIconCollapsed( MooseEnums::UserIconColour eColour )
{
    return icon( maGoodIconsCollapsed, eColour );
}


QPixmap
IconShack::GetGoodUserIconExpanded( MooseEnums::UserIconColour eColour )
{
    return icon( maGoodIconsExpanded, eColour );

}


QPixmap
IconShack::GetExclUserIcon( MooseEnums::UserIconColour eColour )
{
    return icon( maExclIcons, eColour );
}    


QPixmap
IconShack::GetDisabledUserIcon( MooseEnums::UserIconColour eColour )
{
    return icon( maDisabledIcons, eColour );
}


namespace MooseUtils
{
    QString dataPath( QString path ) { return ":/" + path; }
}


void
IconShack::LoadIcons()
{
    maGoodIcons[MooseEnums::eRed].load( MooseUtils::dataPath( "icons/user_red.png" ) );
    maGoodIcons[MooseEnums::eBlue].load( MooseUtils::dataPath( "icons/user_blue.png" ) );
    maGoodIcons[MooseEnums::eGreen].load( MooseUtils::dataPath( "icons/user_green.png" ) );
    maGoodIcons[MooseEnums::eOrange].load( MooseUtils::dataPath( "icons/user_orange.png" ) );
    maGoodIcons[MooseEnums::eBlack].load( MooseUtils::dataPath( "icons/user_black.png" ) );

    maGoodIconsCollapsed[MooseEnums::eRed].load( MooseUtils::dataPath( "icons/user_red32_collapse.png" ) );
    maGoodIconsCollapsed[MooseEnums::eBlue].load( MooseUtils::dataPath( "icons/user_blue32_collapse.png" ) );
    maGoodIconsCollapsed[MooseEnums::eGreen].load( MooseUtils::dataPath( "icons/user_green32_collapse.png" ) );
    maGoodIconsCollapsed[MooseEnums::eOrange].load( MooseUtils::dataPath( "icons/user_orange32_collapse.png" ) );
    maGoodIconsCollapsed[MooseEnums::eBlack].load( MooseUtils::dataPath( "icons/user_black32_collapse.png" ) );
    
    maGoodIconsExpanded[MooseEnums::eRed].load( MooseUtils::dataPath( "icons/user_red32_expand.png" ) );
    maGoodIconsExpanded[MooseEnums::eBlue].load( MooseUtils::dataPath( "icons/user_blue32_expand.png" ) );
    maGoodIconsExpanded[MooseEnums::eGreen].load( MooseUtils::dataPath( "icons/user_green32_expand.png" ) );
    maGoodIconsExpanded[MooseEnums::eOrange].load( MooseUtils::dataPath( "icons/user_orange32_expand.png" ) );
    maGoodIconsExpanded[MooseEnums::eBlack].load( MooseUtils::dataPath( "icons/user_black32_expand.png" ) );

    maExclIcons[MooseEnums::eRed].load( MooseUtils::dataPath( "icons/user_red_caching.png" ) );
    maExclIcons[MooseEnums::eBlue].load( MooseUtils::dataPath( "icons/user_blue_caching.png" ) );
    maExclIcons[MooseEnums::eGreen].load( MooseUtils::dataPath( "icons/user_green_caching.png" ) );
    maExclIcons[MooseEnums::eOrange].load( MooseUtils::dataPath( "icons/user_orange_caching.png" ) );
    maExclIcons[MooseEnums::eBlack].load( MooseUtils::dataPath( "icons/user_black_caching.png" ) );

    maDisabledIcons[MooseEnums::eRed].load( MooseUtils::dataPath( "icons/user_disabled.png" ) );
    maDisabledIcons[MooseEnums::eBlue].load( MooseUtils::dataPath( "icons/user_disabled.png" ) );
    maDisabledIcons[MooseEnums::eGreen].load( MooseUtils::dataPath( "icons/user_disabled.png" ) );
    maDisabledIcons[MooseEnums::eOrange].load( MooseUtils::dataPath( "icons/user_disabled.png" ) );
    maDisabledIcons[MooseEnums::eBlack].load( MooseUtils::dataPath( "icons/user_disabled.png" ) );

    mbIconsLoaded = true;
}

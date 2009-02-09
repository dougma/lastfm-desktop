/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Christian Muehlhaeuser, Last.fm Ltd <chris@last.fm>                *
 *      Max Howell, Last.fm Ltd <max@last.fm>                              *
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

#include "SideBarModel.h"
#include "SideBarDelegate.h"
#include "UglySettings.h"
#include <QFontMetrics>
#include <QModelIndex>
#include <QPainter>


QSize
SideBarDelegate::sizeHint( const QStyleOptionViewItem& option, const QModelIndex& i ) const
{
    int const w = QItemDelegate::sizeHint( option, i ).width();

    using namespace SideBar;

    //small sizes for child items
    if ( i.parent().isValid() )
    {
        if ( The::currentUser().isMetaDataEnabled() )
        {
            // in case we show user avatars
            if ( i.parent().row() == Friends || i.parent().row() == Neighbours )
                return QSize( w, 20 );
        }

        #ifdef Q_WS_MAC
            return QSize( w, 20 );
        #else
            return QSize( w, 17 );
        #endif
    }

    switch (i.row())
    {
        case MyProfile:
            #ifdef LINUX
            return QSize( w, 44 );
            #elif defined Q_WS_MAC
            return QSize( w, 52 );
            #else
            return QSize( w, 38 );
            #endif

        case Spacer1:
            return QSize( w, 14 );

        case Spacer2:
            return QSize( w, 48 );

        case Spacer3:
            return QSize( w, 36 ); //TODO fontmetrics or fixed sizes everywhere

        case StartAStation:
        case NowPlaying:
        case MyRecommendations:
        case PersonalRadio:
        case LovedTracksRadio:
        case NeighbourhoodRadio:
            return QSize( w, 20 );

        default:
            #ifdef Q_WS_MAC
            return QSize( w, 28 );
            #else
            return QSize( w, 20 );
            #endif
    }
}


void
SideBarDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& i ) const
{
    #define LIGHT_BLUE_TEXT QColor( 133, 147, 174 )

    #ifdef Q_WS_MAC
        #define X 21
    #else
        #define X 23
    #endif

    using namespace SideBar;

    bool const is_child_node = i.parent().isValid();
    bool has_children = false;

    if (!is_child_node)
    {
        switch (i.row())
        {
            case RecentlyPlayed:
            case RecentlyLoved:
            case RecentlyBanned:
            case MyTags:
            case Friends:
            case Neighbours:
            case History:
                has_children = true;
                break;
        }

        switch (i.row())
        {
            case Spacer1:
                return;

            case Spacer2:
            {
                int right = option.rect.right();
                int y = option.rect.top() + 13;

                QFont f = option.font; //TODO use model?
                f.setPixelSize( 12 );
                #ifndef Q_WS_MAC
                f.setBold( true );
                #endif

                painter->save();
                painter->setFont( f );
                painter->setPen( QColor( 171, 178, 186 ) );
                painter->drawLine( 0, y, right, y );

                painter->setPen( LIGHT_BLUE_TEXT );
                painter->drawText( X, y + 27, tr("My Stations") );

                painter->restore();
                return;
            }

            case Spacer3:
                QFont f = option.font; //TODO use model?
                f.setPixelSize( 12 );
                #ifndef Q_WS_MAC
                f.setBold( true );
                #endif

                painter->save();
                painter->setFont( f );
                painter->setPen( LIGHT_BLUE_TEXT );
                painter->drawText( X, option.rect.top() + 30, tr("My Profile") );
                painter->restore();
                return;
        }
    }

    #ifndef Q_WS_MAC
        QItemDelegate::paint( painter, option, i );
    #else
        bool const enabled = (option.state & QStyle::State_Enabled) > 0;

        QPalette::ColorGroup cg = enabled
                ? QPalette::Normal
                : QPalette::Disabled;

        painter->setPen( option.palette.color( cg, QPalette::Text ) );
        painter->setFont( i.data( Qt::FontRole ).value<QFont>() );

        QRect rect = option.rect;
        if (has_children) {
            rect.adjust( 0, 0, 0, 28 );
        }
        else if (is_child_node)
            rect.adjust( -20, 0, 0, 20 );

        if (option.state & QStyle::State_Selected)
        {
            if (enabled)
                painter->setPen( Qt::white );
            painter->fillRect( rect, QColor( 0xa4, 0xb4, 0xcb ) );
        }
        else
            painter->eraseRect( rect );

        QRect rect2 = rect;
        int d;
        if (!is_child_node) {
            if (!has_children)
                rect2.adjust( 1, 0, 0, 0 );

            rect2.adjust( 0, 2, 0, 0 );
            d = 24;
        }
        else
        {
            d = 16;
            rect2.adjust( 7, 2, 0, 0 );
        }

        QIcon icon = i.data( Qt::DecorationRole ).value<QIcon>();
        QPixmap pixmap = icon.pixmap( d, d, enabled ? QIcon::Normal : QIcon::Disabled );

        // hannah says don't render the icons with a tint from the selection colour
        QStyleOptionViewItem o = option;
        o.state &= ~QStyle::State_Selected;
        drawDecoration( painter, o, QRect( rect2.topLeft(), pixmap.size() ), pixmap );

        QTextOption noWrap;
        noWrap.setWrapMode( QTextOption::NoWrap );
        rect.adjust( 28, has_children ? 8 : 3, 0, 0 );
        painter->drawText( rect, truncateString( i.data( Qt::DisplayRole ).toString(), m_font, m_sideBarWidth ), noWrap );
    #endif // !Q_WS_MAC
}


QString
SideBarDelegate::truncateString( QString str, QFont font, int sideBarWidth ) const
{
    //TODO pointless? Use QFontMetrics::elidedText()?

    QFontMetrics fm( font );
    Qt::TextElideMode elideMode;

    if ( str.left( 9 ) == "lastfm://" )
        elideMode = Qt::ElideMiddle;
    else
    {
        if ( fm.width( str ) > sideBarWidth - 48 )
        {
            QString tmpStr = str.left( 1 ) + "...";

            for ( int i = str.length(); i > 0; i-- )
            {
                if ( str.mid( i - 1, 1 ) == " " ) continue;

                tmpStr = str.left( i ) + "...";

                if ( fm.width( tmpStr ) < sideBarWidth - 48 )
                    return tmpStr;
            }

            return tmpStr;
        }
        else
            return str;
    }

    return fm.elidedText( str, elideMode, sideBarWidth - 48 );
}


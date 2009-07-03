/*
   Copyright 2005-2009 Last.fm Ltd. 

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

#include "PlayableItemWidget.h"
#include <QLayout>
#include <QLabel>


PlayableItemWidget::PlayableItemWidget(QString stationTitle, const RadioStation& rs)
    : m_rs(rs)
{
    m_rs.setTitle( stationTitle);
    init();
}

PlayableItemWidget::PlayableItemWidget(const RadioStation& rs)
    : m_rs(rs)
{
    init();
}

void
PlayableItemWidget::init()
{
    QString title = fontMetrics().elidedText( m_rs.title(), Qt::ElideRight, 200 );
    if( title != m_rs.title() )
        setToolTip( m_rs.title());
    setText(title);
}

//virtual 
void 
PlayableItemWidget::mouseReleaseEvent(QMouseEvent* event)
{
    emit startRadio(m_rs);
}

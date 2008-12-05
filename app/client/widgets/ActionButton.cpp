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

#include "ActionButton.h"
#include <QAction>


void
ActionButton::setAction( QAction* action )
{
    const bool b = action->isCheckable();
    setCheckable( b );
    
    // only do one or the other or you trigger it all twice
    if (b)
        connect( this, SIGNAL(toggled( bool )), action, SLOT(setChecked( bool )) );
    else
        connect( this, SIGNAL(clicked()), action, SLOT(trigger()) );
    
    connect( action, SIGNAL(changed()), SLOT(onActionChanged()) );
    onActionChanged( action );
}


void
ActionButton::onActionChanged( QAction* action )
{
    if (!action) action = (QAction*) sender();
    setEnabled( action->isEnabled());
    setChecked( action->isChecked());
}

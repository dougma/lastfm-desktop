/***************************************************************************
 *   Copyright 2009 Last.fm Ltd.                                           *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "PlaydarStatus.h"

PlaydarStatus::PlaydarStatus()
:m_state(Connecting)
{
    updateText();
}

void 
PlaydarStatus::onStat(QString name, QString version, QString hostname, bool bAuthenticated)
{
    m_name = name;
    m_version = version;
    m_hostname = hostname;
    m_state = bAuthenticated ? Authenticated : NotAuthenticated;
    updateText();
}

void
PlaydarStatus::onError()
{
    m_state = NotPresent;
    updateText();
}

void
PlaydarStatus::updateText()
{
    switch (m_state) {
        case Connecting: setText("Connecting to Playdar"); break;
        case NotPresent: setText("Playdar not present"); break;
        case NotAuthenticated: setText("Needs Playdar authorisation"); break;
        case Authenticated: setText("Connected to Playdar"); break;
        default: setText("PlaydarStatus::updateText is broken!");
    }
}
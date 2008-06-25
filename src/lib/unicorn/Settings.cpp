/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd                                       *
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

#include "UnicornCommon.h"
#include "Settings.h"
#include <QLocale>


QString
Unicorn::Settings::appLanguage() const
{
    QString const code = Unicorn::QSettings().value( "AppLanguage" ).toString();
    if (code.size())
        return code;

    // If none found, use system locale
#ifdef Q_WS_MAC
    return Unicorn::qtLanguageToLfmLangCode( Unicorn::osxLanguageCode() );
#else
    return Unicorn::qtLanguageToLfmLangCode( QLocale::system().language() );
#endif
}

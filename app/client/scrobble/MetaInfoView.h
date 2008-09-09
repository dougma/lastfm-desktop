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

#include <QWidget>
#include "PlayerState.h"
#include "lib/types/Track.h"


class MetaInfoView : public QWidget
{
    Q_OBJECT
	
	struct
	{
		class QTabBar* tabs;
		class QWebView* web;
	}
	ui;
	
	Track m_track;
	
public:
    MetaInfoView();

public slots:
    void onTrackSpooled( const Track& );

private slots:
    void onLinkClicked( const class QUrl& );
	void onAuthenticationRequired( class QNetworkReply*, class QAuthenticator* );
	void load();

private:
	virtual void resizeEvent( QResizeEvent* );
};

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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QLabel>
#include <QWebView>
#include "PlayerState.h"
#include "lib/types/Track.h"

namespace Unicorn 
{
	class TabWidget;
}


class Bio : public QWebView
{
	Q_OBJECT

public:
	Bio(QWidget *parent = 0);
	void clearContent();
	void setContent(const class CoreDomElement&);

private slots:
	void onLinkClicked(const QUrl&);

private:
	QString cssPath();
};


class MetaInfoView : public QLabel
{
    Q_OBJECT
	
	struct
	{
		Unicorn::TabWidget* infoTabs;
		class Bio* bio;
		class TagListWidget* trackTags;
		class SimilarArtists* similar;
	}
	ui;
	
	Track m_track;

	// the most recent requests (so we don't act on delayed replies)
	WsReply *m_artistInfoReply;
	WsReply *m_artistSimilarReply;
	
public:
    MetaInfoView();
    
    virtual QSize sizeHint() const;

private slots:
    void onTrackSpooled( const Track& );
    void onStateChanged( State );
    void onLinkClicked( const class QUrl& );

	void onArtistInfo( WsReply* );
	void onSimilar( WsReply* );

private:
    QString cssPath();
};

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
 
#ifndef SOURCES_H
#define SOURCES_H

#include <QMainWindow>
#include "SeedTypes.h"
namespace Unicorn{ class TabWidget; }


class Sources : public QWidget
{
	Q_OBJECT

public:
	Sources();
    
    void setupUi();

	struct {
		class Unicorn::TabWidget* tabWidget;
        class SourcesList* stationsBucket;
		class SourcesList* friendsBucket;
		class SourcesList* tagsBucket;
        class RadioControls* controls;
        class QLineEdit* freeInput;
        class QComboBox* inputSelector;
        class ImageButton* cog;
        struct {
            class QAction* iconView;
            class QAction* listView;
            class QAction* firehoseView;
        } actions;
	} ui;
    
    void connectToAmp( class Amp* amp );
    
    QSize sizeHint() const
    {
        return QSize( 366, 216 );
    }
    
protected:
    class WsAccessManager* m_accessManager;
    class Amp* m_connectedAmp;
    class QMenu* m_cogMenu;
	
protected slots:
    void onAmpSeedRemoved( QString, Seed::Type );
    void onUserGetFriendsReturn( class WsReply* );
    void onUserGetTopTagsReturn( class WsReply* );
    void onUserGetPlaylistsReturn( class WsReply* );

    void onAuthUserInfoReturn( class WsReply* );
    void authUserIconDataDownloaded();
    
    void onItemDoubleClicked( const class QModelIndex& index );
    void onDnDAnimationFinished();
    void onFreeInputReturn();
    
    void onAmpDestroyed(){ m_connectedAmp = 0; }

    void onContextMenuRequested( const QPoint& pos );
    void onCogMenuClicked();
    void onCogMenuAction( QAction* );
    void onTabChanged();

};

#endif //SOURCES_H

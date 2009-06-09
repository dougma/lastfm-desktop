/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

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
#ifndef SEEDS_WIDGET_H
#define SEEDS_WIDGET_H

#include <QMainWindow>
#include <QActionGroup>
#include "Seed.h"
namespace Unicorn{ class TabWidget; }


class SeedsWidget : public QWidget
{
	Q_OBJECT

public:
	SeedsWidget();
    
    void setupUi();

	struct {
		class Unicorn::TabWidget* tabWidget;
        class SeedListView* stationsBucket;
		class SeedListView* friendsBucket;
		class SeedListView* tagsBucket;
        class SeedListView* artistsBucket;
        class CombosWidget* combosWidget;
        class RadioControls* controls;
        class QLineEdit* freeInput;
        class QComboBox* inputSelector;
        class ImageButton* cog;
        class QPushButton* dashboard;
        struct {
            class QAction* iconView;
            class QAction* listView;
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

    //QMap< QWidget*, QActionGroup* > m_contextualCogActions;
    
    /** Adds a contextual menu item to the cog menu when the QWidget tab is shown */
    //void addContextualCogAction( QWidget*, QAction* );
	
protected slots:
    void onAmpSeedRemoved( QString, Seed::Type );
    void onUserGetFriendsReturn( class WsReply* );
    void onUserGetTopTagsReturn( class WsReply* );
    void onUserGetPlaylistsReturn( class WsReply* );
    void onUserGetArtistsReturn( WsReply* );

    void onAuthUserInfoReturn( class WsReply* );
    void authUserIconDataDownloaded();
    
    void onItemDoubleClicked( const class QModelIndex& index );
    void onFreeInputReturn();
    
    void onAmpDestroyed(){ m_connectedAmp = 0; }

    void onContextMenuRequested( const QPoint& pos );
    void onCogMenuClicked();
    void onTabChanged();
    
    void onTopArtistsToggled( bool );
    void onRecentArtistsToggled( bool );

};

#endif //SEEDS_WIDGET_H

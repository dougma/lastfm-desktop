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
 
#ifndef AMP_H
#define AMP_H

#include <QWidget>
#include "SeedTypes.h"
#include "lib/lastfm/types/Track.h"
#include "widgets/UnicornWidget.h"
#include "widgets/PlaylistDialog.h"

#include <QMouseEvent>
#include "the/MainWindow.h"
class Amp : public QWidget
{
    Q_OBJECT
public:
    Amp( QWidget* parent = 0 );
    
    /** add the item to the bucket and load any associated data (ie image) */
    void addAndLoadItem( const QString& item, const Seed::Type );
    
    struct
    {
        class ImageButton* love;
        class ImageButton* ban;
        class ImageButton* share;
        class ImageButton* tag;
        class ImageButton* cog;
    } scrobbleRatingUi;
    
public slots:
    void showPlaylistDialog();
    
signals:
    void itemRemoved( QString, Seed::Type );

protected:
    
    struct {
        class PlayerBucketList* bucket;
        class RadioControls* controls;
    } ui;
    
    void resizeEvent( QResizeEvent* );
    void setupUi();
    class QMenu* m_cogMenu;
    Track m_track;
    
    QPoint m_mouseDownPos;
    void mousePressEvent( QMouseEvent* e )
    {
        m_mouseDownPos = e->pos();
    }
    
    void mouseMoveEvent( QMouseEvent* e )
    {
        The::mainWindow().move( e->globalPos() 
                                - m_mouseDownPos 
                                - QPoint( 0, parentWidget()->mapToParent( mapToParent( pos() )).y() 
                                             + The::mainWindow().frameGeometry().height() 
                                             - The::mainWindow().height()) );
    }
  
protected slots:
    void onTrackSpooled( Track );
    void onContextMenuRequested( const QPoint& );

    void onPraiseClientTeam();
    void onCurseClientTeam();
    void onCogMenuClicked();
    
private:
    UNICORN_UNIQUE_DIALOG_DECL( PlaylistDialog );
    
};

#endif //AMP_H
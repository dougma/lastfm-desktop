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
#include <QMouseEvent>


class Amp : public QWidget
{
    Q_OBJECT

public:
    Amp();
    
    /** add the item to the bucket and load any associated data (ie image) */
    void addAndLoadItem( const QString& item, const Seed::Type );
    
    QSize sizeHint() const { return QSize( 366, 86 ); }

    struct {
        class PlayerBucketList* bucket;
        class RadioControls* controls;
        class UnicornVolumeSlider* volume;
        class ImageButton* bucketsButton;
        class ImageButton* dashboardButton;
        class BorderedContainer* borderWidget;
    } ui;

signals:
    void itemRemoved( QString, Seed::Type );

protected:
    virtual void paintEvent( QPaintEvent* );   
    virtual void resizeEvent( QResizeEvent* );
    
    enum AnimationPosition { Left = 0, Right };
    
    void showWidgetAnimated( QWidget* w, AnimationPosition );
    void hideWidgetAnimated( QWidget* w, AnimationPosition );

protected slots:
    void onPlayerBucketChanged();
    void onWidgetAnimationFrameChanged( int );
    
private:
    void setupUi();
    
};

#endif

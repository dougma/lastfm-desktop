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
#ifndef AMP_H
#define AMP_H

#include <QWidget>
#include "Seed.h"
#include "State.h"


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
        class BorderedContainer* borderWidget;
    } ui;

signals:
    void itemRemoved( QString, Seed::Type );

protected:
    virtual void paintEvent( QPaintEvent* );   
    virtual void resizeEvent( QResizeEvent* );

protected slots:
    void onPlayerBucketChanged();
    void onWidgetAnimationFrameChanged( int );
    void onPlayerChanged( const QString& );
    void onStateChanged( State );

private:
    void setupUi();
    void setRadioControlsVisible( bool );
    bool isRadioControlsVisible() const;

    class QTimeLine* m_timeline;
    QString m_playerName;
};

#endif

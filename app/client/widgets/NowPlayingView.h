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

#ifndef NOW_PLAYING_VIEW_H
#define NOW_PLAYING_VIEW_H

#include <QImage>
#include <QWidget>


class NowPlayingView : public QWidget
{
    Q_OBJECT

public:
    NowPlayingView( QWidget *parent = 0 );

private slots:
    void onAppEvent( int, const QVariant& );

private:
    void paintEvent( QPaintEvent* );

    QImage m_cover;
    class QLabel* m_label;
};

#endif // NOW_PLAYING_VIEW_H

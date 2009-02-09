/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Christian Muehlhaeuser, Last.fm Ltd <chris@last.fm>                *
 *      Max Howell, Last.fm Ltd <max@last.fm>                              *
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
#include <QString>

class QLabel;
class QPushButton;
class QTimeLine;


/** The class will automatically animate itself into view after creation. */

class RestStateMessage : public QWidget
{
    Q_OBJECT

public:
    RestStateMessage( class RestStateWidget* );

    void setMessage( const QString& );
    void setAcceptText( const QString& );
    void setFaqHref( const QString& );

signals:
    void moreHelpClicked();
    void accepted();

protected:
    virtual void paintEvent( QPaintEvent* );
    virtual void showEvent( QShowEvent* );

private slots:
    void onSlideStep( int );
    void onSlideFinished();

private:
    QTimeLine* m_timeline;

    struct
    {
        QLabel* icon;
        QLabel* text;
        QPushButton* accept;

    } ui;
};

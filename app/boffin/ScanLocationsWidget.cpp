/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
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

#include "ScanLocationsWidget.h"
#include <QtGui>

//bold scanning now:
// line
//bold queued:
// line
// line


ScanLocationsWidget::ScanLocationsWidget()
{
    QHBoxLayout* h = new QHBoxLayout( this );
    h->addLayout( layout = new QGridLayout( this ) );
    h->addStretch();
}


void
ScanLocationsWidget::setLocations( const QStringList& paths )
{
    this->paths = paths;
    
    QList<QLabel*> labels;
    
    uint row = 0;
    foreach (QString path, paths)
    {
        QLabel* label;
        QPushButton* button;
        layout->addWidget( label = new QLabel(path), row, 0 );
        layout->addWidget( button = new QPushButton(tr("Abort")), row, 1 );
        row++;
        
        labels += label;
    }
    
    QLabel* l = labels.first();
    l->setText( "Scanning: " + l->text() );
}


void
ScanLocationsWidget::setCurrentlyScanning( const QString& path )
{}

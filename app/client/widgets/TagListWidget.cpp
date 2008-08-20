/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Last.fm Ltd <client@last.fm>                                       *
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

#include "TagListWidget.h"
#include "Settings.h"
#include "lib/core/UnicornUtils.h"
#include "lib/types/Tag.h"
#include <QDesktopServices>
#include <QHeaderView>
#include <QUrl>


TagListWidget::TagListWidget( QWidget* parent ) 
        : QTreeWidget( parent )
{
    QTreeWidget::setColumnCount( 3 );
    QTreeWidget::hideColumn( 1 );
    QTreeWidget::hideColumn( 2 );
    QTreeWidget::setRootIsDecorated( false );
    QTreeWidget::header()->hide();

    m_sortOrder = Tags::MostPopularOrder;

    m_actionSortMostPopular = m_sortTagsMenu.addAction( tr( "Sort by Popularity" ) );
    m_actionSortMostPopular->setCheckable( true );

    m_actionSortAZ = m_sortTagsMenu.addAction( tr( "Sort Tags A-Z" ) );
    m_actionSortAZ->setCheckable( true );

    m_actionSortZA = m_sortTagsMenu.addAction( tr( "Sort Tags Z-A" ) );
    m_actionSortZA->setCheckable( true );
    
    m_sortTagsMenu.addSeparator();
    QAction* a = m_sortTagsMenu.addAction( tr("Open Last.fm Page for this Tag") );
    connect( a, SIGNAL(triggered()), SLOT(openTagPageForCurrentItem()) );

    QActionGroup* sortActions = new QActionGroup( this );
    sortActions->addAction( m_actionSortMostPopular );
    sortActions->addAction( m_actionSortAZ );
    sortActions->addAction( m_actionSortZA );

    m_actionSortMostPopular->setChecked( true );

    connect( this,                      SIGNAL( customContextMenuRequested( const QPoint& ) ),
             this,                        SLOT( showSortContextMenu( const QPoint& ) ) );
    connect( m_actionSortAZ,            SIGNAL( triggered() ),
             this,                        SLOT( sortAZ() ) );
    connect( m_actionSortZA,            SIGNAL( triggered() ),
             this,                        SLOT( sortZA() ) );
    connect( m_actionSortMostPopular,   SIGNAL( triggered() ),
             this,                        SLOT( sortMostPopular() ) );
}


void
TagListWidget::showSortContextMenu( const QPoint& point )
{
    m_sortTagsMenu.exec( mapToGlobal( point ) );
}


void
TagListWidget::sortAZ()
{
    sortItems( 1, Qt::AscendingOrder );
    update();

    m_sortOrder = Tags::AscendingOrder;
    m_actionSortAZ->setChecked( true );
}


void
TagListWidget::sortZA()
{
    sortItems( 1, Qt::DescendingOrder );
    update();

    m_sortOrder = Tags::DescendingOrder;
    m_actionSortZA->setChecked( true );
}


void
TagListWidget::sortMostPopular()
{
    sortByColumn( 2, Qt::AscendingOrder );
    update();

    m_sortOrder = Tags::MostPopularOrder;
    m_actionSortMostPopular->setChecked( true );
}


QTreeWidgetItem*
TagListWidget::addItem( QString label )
{
    label = label.toLower();
    
    QTreeWidgetItem *entry = new QTreeWidgetItem;
    entry->setText( 0, label );
    entry->setText( 1, label.toLower() );
    // I couldn't make it sort properly otherwise, even the QVariant methods wouldn't work!
    entry->setText( 2, QString::number( 10 * 1000 + topLevelItemCount() ) );
    addTopLevelItem( entry );

    return entry;
}


void
TagListWidget::addItems( const QStringList& labels )
{
    foreach (QString const label, labels)
        addItem( label );
    sort();
}


void
TagListWidget::sort()
{
    setSortOrder( m_sortOrder );
}


void
TagListWidget::setSortOrder( Tags::SortOrder sortOrder )
{
    if ( sortOrder == Tags::AscendingOrder )
        sortAZ();
    else if ( sortOrder == Tags::DescendingOrder )
        sortZA();
    else if ( sortOrder == Tags::MostPopularOrder )
        sortMostPopular();
}


void
TagListWidget::openTagPageForCurrentItem()
{
	QDesktopServices::openUrl( Tag( currentItem()->text( 0 ) ).url() );
}

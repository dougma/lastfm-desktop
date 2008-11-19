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

#include "SourcesList.h"
#include "PlayableListItem.h"
#include <QStackedLayout>

SourcesList::SourcesList( QWidget* parent )
            : QListWidget( parent ),
              m_customModeEnabled( false )
{
    setViewMode( QListView::IconMode );
    setFlow( QListView::LeftToRight );
    setResizeMode( QListView::Adjust );
    setIconSize( QSize( 36, 38 ) );
    setLayoutMode( QListView::SinglePass );
    setUniformItemSizes( false );
    setDragEnabled( true );
    setDragDropMode( QAbstractItemView::DragOnly );
    setMovement( QListView::Free );
    model()->setSupportedDragActions( Qt::CopyAction );
    setContentsMargins( 5, 5, 5, 5 );
    refresh();
    
    setViewportMargins( 10, 10, 10, 10 );
    
    QStackedLayout* l = new QStackedLayout( this );
    l->addWidget( viewport() );
    connect( model(), SIGNAL( dataChanged( const QModelIndex, const QModelIndex)), SLOT( onDataChanged( const QModelIndex, const QModelIndex )));
}


QMimeData* 
SourcesList::mimeData( const QList<QListWidgetItem *> items ) const
{
    if( items.isEmpty() )
        return 0;
    
    PlayableListItem* item = dynamic_cast<PlayableListItem*>( items.first() );
    
    PlayableMimeData* data = new PlayableMimeData;
    data->setType( item->playableType() );
    data->setText( item->text() );
    data->setImageData( item->icon().pixmap(iconSize()).toImage() );
    
    if( item->playableType() == Seed::PreDefinedType )
        data->setRQL( item->rql() );
    return data;
}


void 
SourcesList::setSourcesViewMode( ViewMode m )
{
    if( m < CustomMode )
    {
        QListView::setViewMode( (QListView::ViewMode)m );
        ((QStackedLayout*)layout())->setCurrentWidget( viewport() );
        setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
        setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    }
    else
    {
        if( m_customWidget )
        {
            m_customModeEnabled = true;
            ((QStackedLayout*)layout())->setCurrentWidget( m_customWidget );
            setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
            setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        }
    }
}


void 
SourcesList::addCustomWidget( QWidget* w )
{
    m_customWidget = w;
    ((QStackedLayout*)layout())->addWidget( m_customWidget );
}

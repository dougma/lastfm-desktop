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
#include <QStyledItemDelegate>

SourcesList::SourcesList( QWidget* parent )
            : QListWidget( parent ),
              m_customModeEnabled( false )
{
    QStackedLayout* l = new QStackedLayout( this );
    
    setItemDelegate( new SourcesListDelegate( this ) );
    setSourcesViewMode( IconMode );
    setFlow( QListView::LeftToRight );
    setResizeMode( QListView::Adjust );
    setIconSize( QSize( 36, 38 ) );
    setLayoutMode( QListView::SinglePass );
    setDragEnabled( true );
    setDragDropMode( QAbstractItemView::DragOnly );
    setAutoScroll( false );
    setUniformItemSizes( false );
    model()->setSupportedDragActions( Qt::CopyAction );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    
    
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
    data->setImageData( item->icon().pixmap( QSize( 36, 38 )).toImage() );
    
    if( item->playableType() == Seed::PreDefinedType )
        data->setRQL( item->rql() );
    return data;
}


void 
SourcesList::setSourcesViewMode( ViewMode m )
{
    if( m < CustomMode )
    {
        if( m == IconMode )
            setSpacing( 5 );
        else
            setSpacing( 0 );
        
        QListView::setViewMode( (QListView::ViewMode)m );
        QStackedLayout* l;
        if( layout() && (l = dynamic_cast<QStackedLayout*>(layout())))
            l->setCurrentWidget( viewport() );
        setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
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

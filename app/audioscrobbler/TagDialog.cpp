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
#include "TagBuckets.h"
#include "TagDialog.h"
#include "TagListWidget.h"
#include "lib/unicorn/widgets/SpinnerLabel.h"
#include "lib/unicorn/widgets/TrackWidget.h"
#include "lib/unicorn/widgets/UnicornTabWidget.h"
#include <lastfm/User>
#include <QtCore>
#include <QtGui>


TagDialog::TagDialog( const Track& track, QWidget *parent )
        : QDialog( parent, Qt::Dialog )
{
    m_track = track;
    
    setupUi();
    
    {
        QNetworkReply* r;
        follow( r = track.getTopTags() );
        ui.suggestedTags->setTagsRequest( r );
        follow( r = track.getTags() );
        connect( r, SIGNAL(finished( WsReply* )), ui.appliedTags->ui.track, SLOT(onGotTags( WsReply* )) );
        follow( r = track.artist().getTags() );
        connect( r, SIGNAL(finished( WsReply* )), ui.appliedTags->ui.artist, SLOT(onGotTags( WsReply* )) );
        follow( r = track.album().getTags() );
        connect( r, SIGNAL(finished( WsReply* )), ui.appliedTags->ui.album, SLOT(onGotTags( WsReply* )) );
    }
    
    
    setWindowTitle( tr("Tag") );
    
    QNetworkReply* r = AuthenticatedUser().getTopTags();
    ui.yourTags->setTagsRequest( r );
    follow( r );
	
    ui.buttons->button( QDialogButtonBox::Ok )->setText( tr("Tag") );


    connect( ui.suggestedTags, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int)), SLOT( onTagListItemDoubleClicked( QTreeWidgetItem*, int)));
    connect( ui.yourTags, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int)), SLOT( onTagListItemDoubleClicked( QTreeWidgetItem*, int)));
    connect( ui.appliedTags, SIGNAL(suggestedTagsRequest( WsReply* )), ui.suggestedTags, SLOT(setTagsRequest( WsReply* )) ); 
    connect( ui.appliedTags, SIGNAL(suggestedTagsRequest( WsReply* )), SLOT(follow( WsReply* )) ); 
    
    connect( ui.buttons, SIGNAL(accepted()), SLOT(accept()) );
    connect( ui.buttons, SIGNAL(rejected()), SLOT(reject()) );
}


void
TagDialog::setupUi()
{
    QLabel* l1, *l2;
    
    QVBoxLayout* v = new QVBoxLayout;
    v->addWidget( l1 = new QLabel( tr( "Suggested Tags" )));
    v->addWidget( ui.suggestedTags = new TagListWidget );
    v->addSpacing( 8 );
    v->addWidget( l2 = new QLabel( tr( "Your Tags" )));
    v->addWidget( ui.yourTags = new TagListWidget );
    v->setSpacing( 4 );

    for (int x = 0; x < v->count(); ++x)
    {
        QWidget* w = v->itemAt( x )->widget();
        if (w) w->setAttribute( Qt::WA_MacSmallSize );
    }

    QHBoxLayout* h = new QHBoxLayout;
    h->addLayout( v );
    h->addSpacing( 12 );
    h->addWidget( ui.appliedTags = new TagBuckets( m_track ) );
    
    v = new QVBoxLayout( this );
    v->addWidget( ui.track = new TrackWidget );
    v->addSpacing( 10 );
    v->addLayout( h );
    v->addWidget( ui.buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel ) );
    
    ui.suggestedTags->setDragEnabled( true );
    ui.yourTags->setDragEnabled( true );
    ui.track->layout()->addWidget( ui.spinner = new SpinnerLabel );
    ui.track->setTrack( m_track );
    ui.spinner->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );
}


void
TagDialog::follow( QNetworkReply* r )
{
    r->setParent( this );
    connect( r, SIGNAL(finished()), SLOT(onWsFinished()) );
    m_activeRequests += r;
    ui.spinner->show();
}


void
TagDialog::accept()
{
    m_track.addTags( ui.appliedTags->ui.track->newTags() );
    m_track.artist().addTags( ui.appliedTags->ui.artist->newTags() );
    m_track.album().addTags( ui.appliedTags->ui.album->newTags() );

    foreach (QString tag, ui.appliedTags->ui.track->deletedTags())
        m_track.removeTag( tag );
    
    QDialog::accept();
}


void
TagDialog::onWsFinished( QNetworkReply *r )
{
    m_activeRequests.removeAll( r );
    ui.spinner->setVisible( m_activeRequests.size() );
}


void
TagDialog::onTagActivated( QTreeWidgetItem *item )
{
}


void
TagDialog::onAddClicked()
{
//    if (currentTagListWidget()->add( ui.edit->text() ))
//    {
//        ui.edit->clear();
//    }
//    else
//        QApplication::beep(); //TODO visually highlight the already entered one
}


void 
TagDialog::removeCurrentTag()
{
//    QShortcut* sc = qobject_cast<QShortcut*>(sender());
//    if( !sc )
//        return;
//    QTreeWidget* list = qobject_cast<QTreeWidget*>(sc->parentWidget());
//    if( !list )
//        return;
//    
//    if( list->hasFocus())
//        delete list->currentItem();
}


void
TagDialog::onTagListItemDoubleClicked( QTreeWidgetItem* item, int)
{
    TagListWidget* w = qobject_cast< TagListWidget*>( sender());
    if( !w )
        return;
    
    QModelIndex i = w->indexFromItem( item, 0 );
    
    QStyleOptionViewItem options;
    options.initFrom( this );
    options.showDecorationSelected = true;
    options.state = QStyle::State_Selected;
    options.rect = w->visualItemRect( item );
    
//TODO    DelegateDragHint* d = new DelegateDragHint( w->itemDelegate( i ), i, options, w );
//TODO    d->setMimeData( PlayableMimeData::createFromTag( Tag( item->text(0)) ) );
    
//TODO    d->dragTo( ui.appliedTags->currentBucket());
}

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

#include "TagDialog.h"
#include "widgets/TagListWidget.h"
#include "widgets/UnicornTabWidget.h"
#include "widgets/UnicornWidget.h"
#include "widgets/TrackWidget.h"
#include "TagBuckets.h"
#include "lib/lastfm/types/User.h"
#include "lib/unicorn/widgets/SpinnerLabel.h"
#include "lib/lastfm/ws/WsReply.h"
#include <QtCore>
#include <QtGui>


TagDialog::TagDialog( const Track& track, QWidget *parent )
        : QDialog( parent, Qt::Dialog )
{
    m_track = track;
    
    setupUi();
    
    {
        WsReply* r;
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
    UnicornWidget::paintItBlack( this );
    
    WsReply* r = AuthenticatedUser().getTopTags();
    ui.yourTags->setTagsRequest( r );
    follow( r );
	
    ui.buttons->button( QDialogButtonBox::Ok )->setText( tr("Tag") );

    connect( ui.suggestedTags, SIGNAL(itemActivated( QTreeWidgetItem*, int )), SLOT(onTagActivated( QTreeWidgetItem* )) );
    connect( ui.yourTags, SIGNAL(itemActivated( QTreeWidgetItem*, int )), SLOT(onTagActivated( QTreeWidgetItem* )) );
    
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
TagDialog::follow( WsReply* r )
{
    r->setParent( this );
    connect( r, SIGNAL(finished( WsReply* )), SLOT(onWsFinished( WsReply* )) );
    m_activeRequests += r;
    ui.spinner->show();
}


void
TagDialog::accept()
{
    m_track.addTags( ui.appliedTags->ui.track->newTags() );
    m_track.artist().addTags( ui.appliedTags->ui.artist->newTags() );
    m_track.album().addTags( ui.appliedTags->ui.album->newTags() );
    QDialog::accept();
}


void
TagDialog::onWsFinished( WsReply *r )
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

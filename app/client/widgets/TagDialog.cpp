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
    
    //    ui.tabs1->setTabEnabled( 0, !track.isNull() );
    //    ui.tabs1->setTabEnabled( 1, !track.artist().isNull() );
    //    ui.tabs1->setTabEnabled( 2, !track.album().isNull() );
    
    {
        WsReply* r;
        follow( r = track.getTopTags() );
        ui.suggestedTags->setTagsRequest( r );
        follow( r = track.getTags() );
        //    ui.trackTags->setTagsRequest( r );
        follow( r = track.artist().getTags() );
        //    ui.artistTags->setTagsRequest( r );
        follow( r = track.album().getTags() );
        //    ui.albumTags->setTagsRequest( r );
    }
    
    
    setWindowTitle( tr("Tag") );
    UnicornWidget::paintItBlack( this );
    
    WsReply* r = AuthenticatedUser().getTopTags();
    ui.yourTags->setTagsRequest( r );
    follow( r );
	
    QRegExp rx( "[a-zA-Z0-9\\-:,' ]{1,255}" );
    ui.edit->setValidator( new QRegExpValidator( rx, this ) );
    ui.edit->setCompleter( new QCompleter( ui.suggestedTags->model() ) );

    ui.buttons->button( QDialogButtonBox::Ok )->setText( tr("Tag") );
    ui.buttons->button( QDialogButtonBox::Ok )->setEnabled( false );

    connect( ui.edit, SIGNAL(returnPressed()), ui.add, SLOT(animateClick()) );
    connect( ui.add, SIGNAL(clicked()), SLOT(onAddClicked()) );

    connect( ui.suggestedTags, SIGNAL(itemActivated( QTreeWidgetItem*, int )), SLOT(onTagActivated( QTreeWidgetItem* )) );
    connect( ui.yourTags, SIGNAL(itemActivated( QTreeWidgetItem*, int )), SLOT(onTagActivated( QTreeWidgetItem* )) );
    
    connect( ui.buttons, SIGNAL(accepted()), SLOT(accept()) );
    connect( ui.buttons, SIGNAL(rejected()), SLOT(reject()) );
}


void
TagDialog::setupUi()
{

//    connect( new QShortcut( QKeySequence(QKeySequence::Delete), ui.trackTags), SIGNAL( activated()), SLOT( removeCurrentTag()));
//    connect( new QShortcut( QKeySequence("Backspace"), ui.trackTags), SIGNAL( activated()), SLOT( removeCurrentTag()));
//    connect( new QShortcut( QKeySequence(QKeySequence::Delete), ui.artistTags), SIGNAL( activated()), SLOT( removeCurrentTag()));
//    connect( new QShortcut( QKeySequence("Backspace"), ui.artistTags), SIGNAL( activated()), SLOT( removeCurrentTag()));
//    connect( new QShortcut( QKeySequence(QKeySequence::Delete), ui.albumTags), SIGNAL( activated()), SLOT( removeCurrentTag()));
//    connect( new QShortcut( QKeySequence("Backspace"), ui.albumTags), SIGNAL( activated()), SLOT( removeCurrentTag()));
    
    ui.tabs2 = new Unicorn::TabWidget;
    ui.tabs2->addTab( tr("Suggested Tags"), ui.suggestedTags = new TagListWidget );
    ui.tabs2->addTab( tr("Your Tags"), ui.yourTags = new TagListWidget );
    
    ui.suggestedTags->setDragEnabled( true );
    ui.yourTags->setDragEnabled( true );
    
    QHBoxLayout* h2 = new QHBoxLayout;
    h2->addWidget( ui.edit = new QLineEdit );
    h2->addWidget( ui.add = new QPushButton( tr("Add") ) );

    QVBoxLayout* v = new QVBoxLayout( this );
    v->addWidget( ui.track = new TrackWidget );
    ui.track->setTrack( m_track );
    v->addLayout( h2 );
    v->addWidget( ui.appliedTags = new TagBuckets( m_track ));
    v->addWidget( ui.tabs2 );
    v->addWidget( ui.buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel ) );
    
    ui.track->layout()->addWidget( ui.spinner = new SpinnerLabel );
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
//    m_track.addTags( ui.trackTags->newTags() );
//    m_track.artist().addTags( ui.artistTags->newTags() );
//    m_track.album().addTags( ui.albumTags->newTags() );
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

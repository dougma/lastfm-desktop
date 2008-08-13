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
#include "Settings.h"
#include "lib/types/User.h"

#include <QTimer>
#include <QPushButton>
#include <QDebug>
#include <QCompleter>
#include <QPainter>
#include <QKeyEvent>



TagDialog::TagDialog( QWidget *parent )
        : QDialog( parent, Qt::Dialog )
{
    ui.setupUi( this );
    ui.tagEdit->setFocus();
    ui.tagEdit->installEventFilter( this );

#if 0 //TODO
    ui.tagTypeBox->setCurrentIndex( user.lastTagType( 1 ) );
    ui.personalTagsList->setSortOrder( (Tags::SortOrder) user.personalTagsListSortOrder() );
    ui.publicTagsList->setSortOrder( (Tags::SortOrder) user.publicTagsListSortOrder() );
#endif
	
    QRegExp rx( "[a-zA-Z0-9\\-:,' ]{1,255}" );
    ui.tagEdit->setValidator( new QRegExpValidator( rx, this ) );

    ui.buttonBox->button( QDialogButtonBox::Ok )->setText( tr("Tag") );
    ui.buttonBox->button( QDialogButtonBox::Ok )->setEnabled( false );

    connect( ui.tagTypeBox, SIGNAL( currentIndexChanged( int ) ), SLOT( onTagTypeChanged( int ) ) );
    connect( ui.personalTagsList, SIGNAL( itemActivated( QTreeWidgetItem*, int ) ), SLOT( onTagActivated( QTreeWidgetItem* ) ) );
    connect( ui.publicTagsList, SIGNAL( itemActivated( QTreeWidgetItem*, int ) ), SLOT( onTagActivated( QTreeWidgetItem* ) ) );
    connect( this, SIGNAL( accepted() ), SLOT( onAccepted() ) );

    connect( ui.buttonBox, SIGNAL( accepted() ), SLOT( accept() ) );
    connect( ui.buttonBox, SIGNAL( rejected() ), SLOT( reject() ) );

    //needs to stay textEdited() not textChanged() or the completion breaks the filtering
    connect( ui.tagEdit, SIGNAL( textEdited( QString ) ), SLOT( onTagEditChanged() ) );

//////
    follow( User( The::settings().username() ).getTopTags() );
}


void
TagDialog::follow( WsReply* r )
{
    r->setParent( this );
    connect( r, SIGNAL(finished( WsReply* )), SLOT(onWsFinished( WsReply* )) );
    m_activeRequests += r;
}


void
TagDialog::setTrack( const Track& track )
{
    m_track = track;
    onTagTypeChanged( ui.tagTypeBox->currentIndex() );

    // We can't tag album if there isn't one
    if ( track.album().isNull() )
        ui.tagTypeBox->removeItem( 2 );

    ui.spinner->show();
}


void
TagDialog::onAccepted()
{
#if 0
    SetTagRequest *request = new SetTagRequest;
    request->setTag( ui.tagEdit->text() );
    request->setArtist( m_track.artist() );

    // we do this because if the originaltags haven't loaded yet we will
    // inadvertedly remove the old tags if we use overwrite mode, but if they
    // did load, the user may have removed some tags, so we must overwrite
    if ( m_originalTags.isEmpty() )
        request->setMode( TAG_APPEND );

    switch ( ui.tagTypeBox->currentIndex() )
    {
        case 0: // artist
            request->setType( UnicornEnums::ItemArtist );
            break;

        case 1: // track
            request->setType( UnicornEnums::ItemTrack );
            request->setToken( m_track.track() );
            break;

        case 2: // album
            request->setType( UnicornEnums::ItemAlbum );
            request->setToken( m_track.album() );
            break;

        default:
            delete request;
            return;
    }

    request->start();
#endif

    saveSettings();
}


void
TagDialog::saveSettings()
{
#if 0
    LastFmUserSettings &user = The::settings().currentUser();
    user.setPersonalTagsListSortOrder( (int)ui.personalTagsList->sortOrder() );
    user.setPublicTagsListSortOrder( (int)ui.publicTagsList->sortOrder() );
    user.setLastTagType( ui.tagTypeBox->currentIndex() );
#endif
}


void
TagDialog::onTagTypeChanged( int type )
{
#if 0
    // cancel any pending requests
    foreach (WsReply* r, m_activeRequests)
        switch (r->type())
        {
            case TypeUserArtistTags:
            case TypeUserTrackTags:
            case TypeUserAlbumTags:
            case TypeArtistTags:
            case TypeTrackTags:
            case TypeAlbumTags:
                r->abort();
                break;

            default:
                break;
        }

    UserArtistTagsRequest* usertags = 0;
    ArtistTagsRequest* publictags = 0;

    switch ( type )
    {
        case 0:
            usertags = requestFactory<UserArtistTagsRequest>();
            publictags = requestFactory<ArtistTagsRequest>();

            ui.metadataLabel->setText( m_track.artist() );
            ui.publicTags->setText( tr( "Popular Tags for this Artist" ) );
            break;

        case 1:
            usertags = requestFactory<UserTrackTagsRequest>();
            ((UserTrackTagsRequest*)usertags)->setTrack( m_track.track() );

            publictags = requestFactory<TrackTagsRequest>();
            ((TrackTagsRequest*)publictags)->setTrack( m_track.track() );

            ui.metadataLabel->setText( m_track.toString() );
            ui.publicTags->setText( tr( "Popular Tags for this Track" ) );
            break;

        case 2:
            usertags = requestFactory<UserAlbumTagsRequest>();
            ((UserAlbumTagsRequest*)usertags)->setAlbum( m_track.album() );

            publictags = requestFactory<AlbumTagsRequest>();
            ((AlbumTagsRequest*)publictags)->setAlbum( m_track.album() );

            ui.metadataLabel->setText( m_track.artist() + ' ' + QChar( 8211 ) /*en dash*/ + ' ' + m_track.album() );
            ui.publicTags->setText( tr( "Popular Tags for this Album" ) );
            break;

        default:
            return;
    }

    usertags->setArtist( m_track.artist() );
    usertags->start();
    publictags->setArtist( m_track.artist() );
    publictags->start();

    // keep what only what the user has typed in themselves
    QStringList tags = ui.tagEdit->text().split( QRegExp( "\\s*,\\s*" ), QString::SkipEmptyParts );
    foreach ( QString const tag, m_originalTags )
        tags.removeAll( tag );

    ui.tagEdit->setText( tags.join( ", " ) );
    m_originalTags.clear();
    ui.publicTagsList->clear();
    ui.spinner->show();
#endif
}


void
TagDialog::onWsFinished( WsReply *request )
{
#if 0
    if ( request->succeeded() )
    {
        TagsRequest *tags = static_cast<TagsRequest*>( request );
        switch ( tags->type() )
        {
            case TypeAlbumTags:
            case TypeArtistTags:
            case TypeTrackTags:
            {
                m_publicTags = tags->tags();
                ui.publicTagsList->addItems( tags->tags() );
            }
            break;

            case TypeUserTags:
            {
                m_userTags = tags->tags();
                ui.personalTagsList->addItems( tags->tags() );
            }
            break;

            case TypeUserArtistTags:
            case TypeUserAlbumTags:
            case TypeUserTrackTags:
            {
                QString current_text = ui.tagEdit->text().trimmed();
                m_originalTags = tags->tags();
                ui.buttonBox->button( QDialogButtonBox::Ok )->setEnabled( !current_text.isEmpty() );
                QStringList tag_edit_tags = QStringList() << m_originalTags << current_text;
                ui.tagEdit->setText( tag_edit_tags.join( ", " ) );
                break;
            }

            default:
                break;
        }
    }

    m_activeRequests.removeAll( request );

    if ( m_activeRequests.isEmpty() )
        ui.spinner->hide();
#endif
}


void
TagDialog::onTagActivated( QTreeWidgetItem *item )
{
    QString const newtag = item->text( 0 ).trimmed();
    QString text = ui.tagEdit->text().trimmed();
    QStringList tags = text.split( QRegExp( "\\s*,\\s*" ), QString::SkipEmptyParts );

    if ( tags.count() && !text.endsWith( ',' ) ) //mxcl knows why
        tags.pop_back();

    tags << newtag << ""; // adds a trailing ", "

    text = tags.join( ", " );
    ui.tagEdit->setText( text );

    onTagEditChanged(); //reset filtering
}


void
TagDialog::onTagEditChanged()
{
    QStringList l1 = m_originalTags;
    QStringList l2 = ui.tagEdit->text().split( QRegExp( "\\s*,\\s*" ), QString::SkipEmptyParts );

    l1.sort();
    l2.sort();

    ui.buttonBox->button( QDialogButtonBox::Ok )->setEnabled( l1 != l2 );
    searchAsYouType( ui.tagEdit->text() );
}


static inline QStringList filterView( TagListWidget* view, const QStringList& tags,
                                      const QString& base, const QString& term )
{
    QRegExp const rx_filter( "*" + term + "*", Qt::CaseInsensitive, QRegExp::Wildcard );
    QRegExp const rx_completion( term + "*", Qt::CaseInsensitive, QRegExp::Wildcard );

    QStringList comps;
    QTreeWidgetItem* current_item = view->currentItem();
    QString current_text;
    if (current_item)
        current_text = current_item->text( 0 );
    current_item = 0;
    view->clear();
    foreach (QString const tag, tags)
    {
        if (rx_filter.exactMatch( tag ))
        {
            //NOTE hiding items would be nicer, but it's reaaally slow
            QTreeWidgetItem* item = view->addItem( tag );
            if (tag == current_text)
                current_item = item;
        }
        if (rx_completion.exactMatch( tag ))
            comps << base + tag;
    }
    view->sort(); //applies user selected custom sorting
    if (current_item)
        view->setCurrentItem( current_item );
    return comps;
}


void
TagDialog::searchAsYouType( const QString& text )
{
    QStringList const tags = text.split( QRegExp( "\\s*,\\s*" ) );
    QString const term = tags.count() ? tags.back().toLower() : "";
    QString const base = text.left( text.length() - term.length() );
    QStringList comps;

    setUpdatesEnabled( false );
    comps += filterView( ui.publicTagsList, m_publicTags, base, term );
    comps += filterView( ui.personalTagsList, m_userTags, base, term );
    setUpdatesEnabled( true );

    if ( term.size() )
    {
        QCompleter *completer = new QCompleter( comps );
        completer->setCaseSensitivity( Qt::CaseInsensitive );
        completer->setCompletionMode( QCompleter::InlineCompletion );
        ui.tagEdit->setCompleter( completer );
    }
}


bool
TagDialog::eventFilter( QObject* o, QEvent* e )
{
    /** used to put the greyed-out help-text in the tagEdit */

    if ( o == ui.tagEdit )
    {
        switch ( e->type() )
        {
            case QEvent::FocusIn:
            {
                QString const text = ui.tagEdit->text();
                if ( text.size() )
                {
                    QRegExp rx( "^.*\\s*,\\s*$" );
                    if ( !rx.exactMatch( text ) )
                        ui.tagEdit->setText( text + ", " );
                }
            }
            break;

            case QEvent::Paint:
                if ( ui.tagEdit->text().isEmpty() )
                {
                    QString const k_text = tr( "Enter comma-separated tags here" );

                    ui.tagEdit->event( e );

                    QRect r = ui.tagEdit->rect().adjusted( 5, 2, -5, 0 );
                    QPainter p( ui.tagEdit );
                    p.setPen( Qt::gray );
                    p.setFont( ui.tagEdit->font() );
                    p.drawText( r, Qt::AlignVCenter, k_text );
                    ui.tagEdit->setMinimumWidth( p.fontMetrics().width( k_text ) + 12 );

                    return true; //eat event
                }
                break;

            case QEvent::KeyPress:
                if ( static_cast<QKeyEvent*>(e)->key() == Qt::Key_Tab )
                {
                    QString text = ui.tagEdit->text().trimmed();
                    if ( text.size() && !text.endsWith( ',' ) )
                    {
                        text += ", ";
                        ui.tagEdit->setText( text );
                        searchAsYouType( text );
                        return true; //eat event if we did some completion
                    }
                }
                break;

            default:
                break;
        }
    }

    return QWidget::eventFilter( o, e );
}

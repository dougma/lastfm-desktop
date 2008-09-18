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
#include "lib/types/User.h"

#include "lib/unicorn/widgets/SpinnerLabel.h"
#include "widgets/TagListWidget.h"

#include <QtCore>
#include <QtGui>


TagDialog::TagDialog( QWidget *parent )
        : QDialog( parent, Qt::Dialog )
{
    setupUi();
    setWindowTitle( tr("Tag") );

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
    
    connect( this, SIGNAL(accepted()), SLOT(onAccepted()) );
    connect( ui.buttons, SIGNAL(accepted()), SLOT(accept()) );
    connect( ui.buttons, SIGNAL(rejected()), SLOT(reject()) );
}


namespace Moose
{
    class TabBar : public QTabBar
    {
    public:
        TabBar()
        {
        #ifdef Q_WS_MAC
            QFont f = font();
            f.setPixelSize( 11 );
            setFont( f );
        #endif
        }
        
    protected:
        virtual void paintEvent( QPaintEvent* e )
        {
            QPainter p( this );
            p.fillRect( rect(), QBrush( QPixmap(":/controls/inactive/tab.png") ) );
            
            int w = width() / count();
            for (int i = 0; i < count(); ++i)
            {
                int const x = i*w;
                
                if (i == count() - 1)
                    w += width() % w;
                
                if (currentIndex() == i)
                    p.fillRect( x, 0, w, height(), QBrush( QPixmap(":/controls/active/tab.png") ) );
                
                p.drawText( x, 0, w, height(), Qt::AlignCenter, tabText( i ) );
            }
        }
    };
    
    class TabWidget : public QWidget
    {
        QStackedWidget* stack;
        Moose::TabBar* bar;

    public:
        TabWidget()
        {
            QVBoxLayout* v = new QVBoxLayout( this );
            v->addWidget( bar = new TabBar );
            v->addWidget( stack = new QStackedWidget );
            v->setSpacing( 0 );
            v->setMargin( 0 );
            connect( bar, SIGNAL(currentChanged( int )), stack, SLOT(setCurrentIndex( int )) );
        }
        
        void addTab( const QString& title, QWidget* w )
        {
            bar->addTab( title );
            stack->addWidget( w );
        }
        
        QWidget* currentWidget() const { return stack->currentWidget(); }
    };
}


void
TagDialog::setupUi()
{
    QPalette p = palette();
    p.setBrush( QPalette::Window, QColor( 0x18, 0x18, 0x19 ) );
    p.setBrush( QPalette::WindowText, QColor( 0xff, 0xff, 0xff, 40 ) );
    setPalette( p );
    
    ui.tabs1 = new Moose::TabWidget;
    ui.tabs1->addTab( tr("Track"), ui.trackTags = new TagIconView );
    ui.tabs1->addTab( tr("Artist"), ui.artistTags = new TagIconView );
    ui.tabs1->addTab( tr("Album"), ui.albumTags = new TagIconView );
    
    ui.tabs2 = new Moose::TabWidget;
    ui.tabs2->addTab( tr("Suggested Tags"), ui.suggestedTags = new TagListWidget );
    ui.tabs2->addTab( tr("Your Tags"), ui.yourTags = new TagListWidget );
        
    QHBoxLayout* h1 = new QHBoxLayout;
    h1->addWidget( ui.cover = new QLabel );
    h1->addWidget( ui.track = new QLabel );
    h1->addWidget( ui.spinner = new SpinnerLabel );
    
    ui.cover->setScaledContents( true );
    ui.spinner->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );
    ui.track->setTextFormat( Qt::RichText );
    
    QHBoxLayout* h2 = new QHBoxLayout;
    h2->addWidget( ui.edit = new QLineEdit );
    h2->addWidget( ui.add = new QPushButton( tr("Add") ) );
    
    QVBoxLayout* v = new QVBoxLayout( this );
    v->addLayout( h1 );
    v->addLayout( h2 );
    v->addWidget( ui.tabs1 );
    v->addWidget( ui.tabs2 );
    v->addWidget( ui.buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel ) );
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
TagDialog::setTrack( const Track& track )
{
    m_track = track;

    QString title = track.title();
    QString artist = track.artist();
    QString album = track.album();
    if (title.isEmpty()) title = '[' + tr("Unknown Track") + ']';
    if (track.duration()) title += " </b>(" + track.durationString() + ')';
    if (album.size()) album = tr("from %1").arg( "<span style='color:#fff'>" + album );
    if (artist.isEmpty()) artist = '[' + tr("Unknown Artist") + ']';
    artist = tr("by %1").arg( "<span style='color:#fff'>" + artist + "</span>" );
    
    ui.track->setText( QString("<div style='color:#fff;margin-bottom:2px'><b>") + 
                #ifdef Q_WS_MAC
                      "<span style='font-size:15pt'>" + 
                #endif
                       title + "</b></div>"
                #ifdef Q_WS_MAC
                      "<span style='font-size:10pt'>" + 
                #endif
                      "<span style='color:#999'><div style='margin-bottom:3px'>" + artist + "</div><div>" + album );

    // We can't tag album if there isn't one
    if (track.album().isNull())
//        ui.albumTags->setEnabled( false );
        ;
    
    int const h = ui.track->sizeHint().height();
    ui.cover->setFixedSize( h, h );
    
    WsReply* r;
    
    follow( r = track.getTopTags() );
    ui.suggestedTags->setTagsRequest( r );
    follow( r = track.getTags() );
    ui.trackTags->setTagsRequest( r );
    follow( r = track.artist().getTags() );
    ui.artistTags->setTagsRequest( r );
    follow( r = track.album().getTags() );
    ui.albumTags->setTagsRequest( r );
    
    connect( new AlbumImageFetcher( track.album(), Album::Medium ), SIGNAL(finished( QByteArray )), SLOT(onCoverDownloaded( QByteArray )) );
}


void
TagDialog::onAccepted()
{
#if 0
    SetTagRequest *request = new SetTagRequest;
    request->setTag( ui.edit->text() );
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
}


#if 0
void
TagDialog::onTagTypeChanged( int type )
{
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
    QStringList tags = ui.edit->text().split( QRegExp( "\\s*,\\s*" ), QString::SkipEmptyParts );
    foreach ( QString const tag, m_originalTags )
        tags.removeAll( tag );

    ui.edit->setText( tags.join( ", " ) );
    m_originalTags.clear();
    ui.publicTagsList->clear();
    ui.spinner->show();
}
#endif


void
TagDialog::onWsFinished( WsReply *r )
{
    m_activeRequests.removeAll( r );
    ui.spinner->setVisible( m_activeRequests.size() );
}


void
TagDialog::onTagActivated( QTreeWidgetItem *item )
{
    QString const newtag = item->text( 0 ).trimmed();
    currentTagListWidget()->add( newtag );
}

void
TagDialog::onAddClicked()
{
    currentTagListWidget()->add( ui.edit->text() );
    ui.edit->clear();
}


TagListWidget*
TagDialog::currentTagListWidget() const
{
    return static_cast<TagListWidget*>(ui.tabs1->currentWidget());
}


void
TagDialog::onCoverDownloaded( const QByteArray& data )
{
    QPixmap p;
    p.loadFromData( data );
    ui.cover->setPixmap( p );
}

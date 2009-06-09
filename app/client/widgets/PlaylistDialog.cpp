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
#include "PlaylistDialog.h"
#include "FirehoseDelegate.h"
#include "TrackWidget.h"
#include "UnicornWidget.h"
#include "widgets/HelpTextLineEdit.h"
#include "app/moose.h"
#include <lastfm/WsReply>
#include <lastfm/User>
#include <lastfm/Playlist>
#include <QtGui>

struct UserPlaylistsModel : QAbstractItemModel
{
    virtual QVariant data(const QModelIndex &index, int role) const
    {
        int const row = index.row();
        
        if (!index.isValid() || row >= ids.count())
            return QVariant();
        
        switch (role)
        {
            case Qt::DisplayRole: return titles[row];
            case Qt::ToolTipRole: return descriptions[row];
            case moose::IdentityRole: return ids[row];
            case moose::SecondaryDisplayRole: 
                return sizes[row] == -1
                        ? tr( "Creating playlist..." )
                        : tr( "%n tracks", "", sizes[row] );
            case moose::SmallDisplayRole: 
                return durations[row] == 0
                        ? ""
                        : Track::durationString( durations[row] );
        }
        
        return QVariant();
    }
    
    virtual Qt::ItemFlags flags( const QModelIndex& i ) const
    {
        // using data means I get bounds checking
        return data( i, moose::IdentityRole ).toInt() == -1
                ? Qt::NoItemFlags
                : QAbstractItemModel::flags( i );
    }
    
    virtual QModelIndex index( int row, int column = 0, const QModelIndex& = QModelIndex() ) const { return createIndex( row, column ); }
    virtual QModelIndex parent(const QModelIndex& ) const { return QModelIndex(); }
    virtual int rowCount( const QModelIndex& ) const { return ids.count(); }
    virtual int columnCount( const QModelIndex& ) const { return 1; }    
    
    QList<int> ids;
    QList<QUrl> urls;
    QList<QString> titles;
    QList<QString> descriptions;
    QList<int> durations;
    QList<int> sizes;
    
    using QAbstractItemModel::beginInsertRows;
    using QAbstractItemModel::endInsertRows;
    using QAbstractItemModel::dataChanged;
};



struct PlaylistsView : QListView
{
    QString text;
    
    virtual void paintEvent( QPaintEvent* e )
    {
        if (model()->rowCount() == 0)
        {            
            QPainter p( viewport() );
            
            QFont f = p.font();
            f.setBold( true );
            f.setPixelSize( 16 ); // indeed pixels are fine on mac and windows, not linux though
            p.setFont( f );
            p.setPen( QColor( 0x646464 ) );
            p.drawText( rect().adjusted( 5, 5, -5, -5 ),
                        Qt::AlignCenter, 
                        text );
        }
        else
            QListView::paintEvent( e );
    }
};



PlaylistDialog::PlaylistDialog( const Track& t, QWidget* parent )
              : QDialog( parent )
{
	setupUi();
    setWindowTitle( tr("Add to Playlist") );

    m_track = t;
    ui.track->setTrack( t );	
    
    delete ui.playlists->itemDelegate();
    ui.playlists->setItemDelegate( new FirehoseDelegate );
    ui.playlists->setModel( m_model = new UserPlaylistsModel );
    
	WsReply* r = AuthenticatedUser().getPlaylists();
	connect( r, SIGNAL(finished( WsReply* )), SLOT(onGotUserPlaylists( WsReply* )) );
}


void
PlaylistDialog::setupUi()
{
    QPushButton* create;
    
	QVBoxLayout* v = new QVBoxLayout( this );
    v->addWidget( ui.track = new TrackWidget );
    v->addSpacing( 8 );
	v->addWidget( ui.playlists = new PlaylistsView );
    v->addWidget( create = new QPushButton( tr("Create New Playlist") ) );
	v->addWidget( ui.buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel ) );
    
    ui.playlists->text = tr("Loading playlists...");
    
	ui.buttons->button( QDialogButtonBox::Ok )->setText( tr("Add") );
    connect( ui.buttons, SIGNAL(accepted()), SLOT(accept()) );
    connect( ui.buttons, SIGNAL(rejected()), SLOT(reject()) );
	
    connect( create, SIGNAL(clicked()), SLOT(create()) );
    
	UnicornWidget::paintItBlack( this );
	UnicornWidget::paintItBlack( ui.playlists );
}


struct PlaylistMetaData
{
    QString title, description;
    QUrl url;
    int duration, size, id;
};


static QList<PlaylistMetaData> meh( WsReply* r )
{
    QList<PlaylistMetaData> playlists;
    
    try
	{
		foreach (WsDomElement e, r->lfm()["playlists"].children( "playlist" ))
		{
            try
            {
                PlaylistMetaData p;
                p.title = e["title"].text();
                p.url = e["url"].text();
                p.id = e["id"].text().toInt();
                p.size = e["size"].text().toInt();
                p.duration = e["duration"].text().toInt();
                p.description = e["description"].text();
                playlists += p;
            }
            catch (std::runtime_error& e)
            {
                qWarning() << e.what();
            }   
		}
	}
	catch (std::runtime_error& e)
	{
		qWarning() << e.what();
	}
    
    return playlists;
}


void
PlaylistDialog::onGotUserPlaylists( WsReply* r )
{
    // done via meh as we have to call endRows() and we must have inserted
    // the number of rows we promised to, so we had to use a struct intermediate
    // as there is no way to back-track
    QList<PlaylistMetaData> playlists = meh( r );

    if (playlists.count() == 0)
    {
        ui.playlists->text = tr("You have no playlists");
        return;
    }
    
    int const n = m_model->ids.size() - 1;
    m_model->beginInsertRows( QModelIndex(), n, n + playlists.size() - 1 );
    
    foreach (PlaylistMetaData p, playlists)
    {
        m_model->ids += p.id;
        m_model->titles += p.title;
        m_model->descriptions += p.description;
        m_model->urls += p.url;
        m_model->durations += p.duration;
        m_model->sizes += p.size;
    }
    
    m_model->endInsertRows();
}


void
PlaylistDialog::accept()
{
    QVariant id = ui.playlists->currentIndex().data( moose::IdentityRole );
    if (id.isValid())
        Playlist( id.toInt() ).addTrack( m_track );
    QDialog::accept();
}


void
PlaylistDialog::create()
{
    QDialogButtonBox* buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    QLineEdit* title, *description;
    
    QDialog d( this );
    d.setWindowTitle( tr( "Create New Playlist" ) );
    QVBoxLayout* v = new QVBoxLayout( &d );
    v->addWidget( title = new HelpTextLineEdit( "Title" ) );
    v->addWidget( description = new HelpTextLineEdit( "Description" ) );
    v->addWidget( buttons );
    
    buttons->button( QDialogButtonBox::Ok )->setText( tr("Create Playlist") );
    connect( buttons, SIGNAL(accepted()), &d, SLOT(accept()) );
    connect( buttons, SIGNAL(rejected()), &d, SLOT(reject()) );
    
    if (d.exec() == QDialog::Accepted)
    {
        int const n = m_model->ids.size();
        
        m_model->beginInsertRows( QModelIndex(), n, n );
        m_model->titles += title->text();
        m_model->descriptions += description->text();
        m_model->ids += -1;
        m_model->urls += QUrl();
        m_model->durations += 0;
        m_model->sizes += -1;
        m_model->endInsertRows();
        
        WsReply* reply = Playlist::create( title->text(), description->text() );
        connect( reply, SIGNAL(finished( WsReply* )), SLOT(onPlaylistCreated( WsReply* )) );
        reply->setAssociatedData( n );
    }
}


void
PlaylistDialog::onPlaylistCreated( WsReply* reply )
{
    int const i = reply->associatedData().toInt();
    
    try
    {
        WsDomElement e = reply->lfm()["playlists"]["playlist"];
        m_model->ids[i] = e["id"].text().toInt();
        m_model->urls[i] = e["url"].text();
        m_model->sizes[i] = 0;
        QModelIndex index = m_model->index( i );
        emit m_model->dataChanged( index, index );
    }
    catch (std::runtime_error& e)
    {
        m_model->descriptions[i] = tr("Error creating playlist");
        qWarning() << e.what();
    }
}

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
#include "BatchScrobbleDialog.h"
#include "app/twiddly/IPodScrobble.h"
#include <QString>
#include <QDebug>
#include <QDesktopWidget>
#include <QDir>


BatchScrobbleDialog::BatchScrobbleDialog( QWidget* parent )
        : QDialog( parent )
{
    ui.setupUi( this );
}


void
BatchScrobbleDialog::setTracks( const QList<Track>& tracks )
{
    m_tracks = tracks;

    addTracksToView();
    
    // work around qt 4.3.x bug where on mac the window control buttons look
    // bizarre
    setWindowModality( Qt::ApplicationModal );

////// This section sizes the dialog as best as possible, it is a shame but
    // Qt sizeHints usually ignore the view contents size, so we do it manually.

    //this magnificent hack bought to you by teh mxcl
    // @short allow access to this protected function so we can determine the
    // ideal width for the confirm dialog
    struct NoProtection : QAbstractItemView
    {
        using QAbstractItemView::sizeHintForColumn;
    };

    int w = 0, desiredwidth = 0;
    for (int x = 0; x < ui.tracksWidget->columnCount(); ++x) {
        desiredwidth = reinterpret_cast<NoProtection*>(ui.tracksWidget)->sizeHintForColumn( x );
        w += desiredwidth;
    }

    ui.tracksWidget->setMinimumWidth( w );
    if (m_tracks.count() > 10)
        ui.tracksWidget->setMinimumHeight( ui.tracksWidget->sizeHint().height() * 2 );

    //make us always the right size
    layout()->setSizeConstraint( QLayout::SetMinimumSize );

    int const W = QDesktopWidget().availableGeometry().width();
    if (sizeHint().width() > W)
        ui.tracksWidget->setMinimumWidth( W
                                          - (sizeHint().width() - ui.tracksWidget->width()) 
                                          - 10 /*small aesthetic gap*/ );

    ui.tracksWidget->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    ui.buttonBox->button( QDialogButtonBox::Ok )->setText( tr("Scrobble") );

///////
    connect( ui.buttonBox, SIGNAL( accepted() ), SLOT( accept() ) );
    connect( ui.buttonBox, SIGNAL( rejected() ), SLOT( reject() ) );
    connect( ui.toggle,    SIGNAL( clicked() ),  SLOT( toggleChecked() ) );

///////
    activateWindow();
}


void
BatchScrobbleDialog::addTracksToView()
{
    QList<QTreeWidgetItem*> items;
    int index = 0;
    int total = 0;
    foreach (IPodScrobble t, m_tracks)
    {
        QTreeWidgetItem* widget = new QTreeWidgetItem;
        widget->setData( 0, Qt::DisplayRole, QString(t.artist()) );
        widget->setData( 1, Qt::DisplayRole, QString(t.title()) );
        widget->setData( 2, Qt::DisplayRole, t.timestamp() );
        widget->setData( 3, Qt::DisplayRole, QString::number(t.playCount()) );

        widget->setFlags( widget->flags() | Qt::ItemIsUserCheckable );
        widget->setCheckState( 0, Qt::Checked );

        widget->setData( 0, Qt::UserRole, index++ );

        items += widget;
        
        total += t.playCount();
    }

    ui.tracksWidget->insertTopLevelItems( 0, items );
    ui.tracksWidget->resizeColumnToContents( 0 );
    ui.tracksWidget->resizeColumnToContents( 1 );
    ui.tracksWidget->resizeColumnToContents( 2 );
    ui.tracksWidget->resizeColumnToContents( 4 );

    ui.tracksWidget->setSortingEnabled( true );
    ui.tracksWidget->sortByColumn( 2, Qt::DescendingOrder );

    QString text = tr( "<p>Last.fm found %n scrobbles on your iPod.", "", total );
    ui.messageLabel->setText( text );
}


void
BatchScrobbleDialog::toggleChecked()
{
    for (int x = 0; x < ui.tracksWidget->topLevelItemCount(); ++x) 
    {
        QTreeWidgetItem* i = ui.tracksWidget->topLevelItem( x );
        i->setCheckState( 0, i->checkState( 0 ) == Qt::Checked ? Qt::Unchecked : Qt::Checked );
    }
}


QList<Track>
BatchScrobbleDialog::tracks() const
{
    QList<Track> tracks;
    for (int x = 0; x < ui.tracksWidget->topLevelItemCount(); ++x)
    {
        QTreeWidgetItem* item = ui.tracksWidget->topLevelItem( x );
        if (item->checkState( 0 ) == Qt::Checked)
        {
            int const index = item->data( 0, Qt::UserRole ).toInt();
            tracks += m_tracks[index];
        }
    }
    return tracks;
}


void
BatchScrobbleDialog::showEvent( QShowEvent* e )
{
    if (!parentWidget()->isActiveWindow()) 
        // qApp alert is bugged, it shouldn't bounce if we're already active!
        //FIXME I couldn't do this->isActiveWindow() as we aren't active yet
        //      so this code breaks if we're the active application on osx
        //      but the window is hidden :(
        qApp->alert( this );
        
    QDialog::showEvent( e );
}

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

#include "MiniNowPlayingView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

MiniNowPlayingView::MiniNowPlayingView( const Track& t )
{
	setupUi();
	ui.artist->setText( t.artist() );
	ui.album->setText( t.album() );
	ui.track->setText( t.title() );
	
	//Load Album Image
	QObject* o = new AlbumImageFetcher( t.album(), Album::Medium );
	connect( o, SIGNAL(finished( QByteArray )), SLOT(onAlbumImageDownloaded( QByteArray )) );
}


void
MiniNowPlayingView::onAlbumImageDownloaded( QByteArray b )
{
	QPixmap p;
	p.loadFromData( b );
	ui.albumArt->setPixmap( p );
}


void
MiniNowPlayingView::setupUi()
{
	
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;

	
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    horizontalLayout->setContentsMargins(12, 0, 0, 0);
	
    ui.albumArt = new QLabel( this );
    ui.albumArt->setObjectName(QString::fromUtf8("albumArt"));
    QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Maximum);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(ui.albumArt->sizePolicy().hasHeightForWidth());
	
    ui.albumArt->setSizePolicy(sizePolicy1);
    ui.albumArt->setMinimumSize(QSize(0, 0));
    ui.albumArt->setMaximumSize(QSize(50, 50));
    ui.albumArt->setFrameShape(QFrame::NoFrame);
    ui.albumArt->setScaledContents(true);
    ui.albumArt->setAlignment(Qt::AlignCenter);
    ui.albumArt->setMargin(0);
    ui.albumArt->setIndent(0);
	
    horizontalLayout->addWidget(ui.albumArt);
	
	
    verticalLayout = new QVBoxLayout();
    verticalLayout->setSpacing(0);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    verticalLayout->setSizeConstraint(QLayout::SetMaximumSize);
    verticalLayout->setContentsMargins(-1, 12, 0, 12);
    ui.artist = new QLabel( this );
    ui.artist->setObjectName(QString::fromUtf8("artist"));
    QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Maximum);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(ui.artist->sizePolicy().hasHeightForWidth());
    ui.artist->setSizePolicy(sizePolicy2);

    QFont font;
    font.setPointSize(10);
    ui.artist->setFont(font);
	
    verticalLayout->addWidget(ui.artist);
	
    ui.track = new QLabel( this );
    ui.track->setObjectName(QString::fromUtf8("track"));
    sizePolicy2.setHeightForWidth(ui.track->sizePolicy().hasHeightForWidth());
    ui.track->setSizePolicy(sizePolicy2);
    ui.track->setFont(font);
	
    verticalLayout->addWidget(ui.track);
	
    ui.album = new QLabel( this );
    ui.album->setObjectName(QString::fromUtf8("album"));
    sizePolicy2.setHeightForWidth(ui.album->sizePolicy().hasHeightForWidth());
    ui.album->setSizePolicy(sizePolicy2);
    ui.album->setFont(font);
	
    verticalLayout->addWidget(ui.album);
    horizontalLayout->addLayout(verticalLayout);
	
	setLayout( horizontalLayout );
	
}